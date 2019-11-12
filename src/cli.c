/*
 * QuickJS + libuv stand alone interpreter
 *
 * Copyright (c) 2019-present Saúl Ibarra Corretgé
 * Copyright (c) 2017-2018 Fabrice Bellard
 * Copyright (c) 2017-2018 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "private.h"
#include "tjs.h"
#include "version.h"

#include <string.h>


static int eval_buf(JSContext *ctx, const void *buf, int buf_len, const char *filename, int eval_flags) {
    JSValue val;
    int ret;

    val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
    if (JS_IsException(val)) {
        tjs_dump_error(ctx);
        ret = -1;
    } else {
        ret = 0;
    }
    JS_FreeValue(ctx, val);
    return ret;
}

void help(void) {
    printf("tjs version %s\n"
           "usage: tjs [options] [file]\n"
           "-h  --help                       list options\n"
           "-e  --eval EXPR                  evaluate EXPR\n"
           "-i  --interactive                go to interactive mode\n"
           "    --strict-module-detection    only run code as a module if its extension is \".mjs\"\n"
           "    --override-filename FILENAME override filename in error messages\n"
           "-q  --quit                       just instantiate the interpreter and quit\n",
           tjs_version());
    exit(1);
}

int main(int argc, char **argv) {
    TJSRuntime *qrt;
    JSContext *ctx;
    int optind;
    char *expr = NULL;
    char *override_filename = NULL;
    int interactive = 0;
    int empty_run = 0;
    int strict_module_detection = 0;

    TJS_SetupArgs(argc, argv);

    /* cannot use getopt because we want to pass the command line to
       the script */
    optind = 1;
    while (optind < argc && *argv[optind] == '-') {
        char *arg = argv[optind] + 1;
        const char *longopt = "";
        /* a single - is not an option, it also stops argument scanning */
        if (!*arg)
            break;
        optind++;
        if (*arg == '-') {
            longopt = arg + 1;
            arg += strlen(arg);
            /* -- stops argument scanning */
            if (!*longopt)
                break;
        }
        for (; *arg || *longopt; longopt = "") {
            char opt = *arg;
            if (opt)
                arg++;
            if (opt == 'h' || opt == '?' || !strcmp(longopt, "help")) {
                help();
                continue;
            }
            if (opt == 'e' || !strcmp(longopt, "eval")) {
                if (*arg) {
                    expr = arg;
                    break;
                }
                if (optind < argc) {
                    expr = argv[optind++];
                    break;
                }
                fprintf(stderr, "tjs: missing expression for -e\n");
                exit(2);
            }
            if (!strcmp(longopt, "override-filename")) {
                if (*arg) {
                    override_filename = arg;
                    break;
                }
                if (optind < argc) {
                    override_filename = argv[optind++];
                    break;
                }
                fprintf(stderr, "tjs: missing expression for --override-filename\n");
                exit(2);
            }
            if (opt == 'i' || !strcmp(longopt, "interactive")) {
                interactive++;
                continue;
            }
            if (opt == 'q' || !strcmp(longopt, "quit")) {
                empty_run++;
                continue;
            }
            if (strcmp(longopt, "strict-module-detection")) {
                strict_module_detection = 1;
            }
            if (opt) {
                fprintf(stderr, "tjs: unknown option '-%c'\n", opt);
            } else {
                fprintf(stderr, "tjs: unknown option '--%s'\n", longopt);
            }
            help();
        }
    }

    qrt = TJS_NewRuntime();
    ctx = TJS_GetJSContext(qrt);

    if (!empty_run) {
        if (expr) {
            if (eval_buf(ctx, expr, strlen(expr), "<cmdline>", 0))
                goto fail;
        } else if (optind >= argc) {
            /* interactive mode */
            interactive = 1;
        } else {
            const char *filename;
            filename = argv[optind];
            int flags = JS_EVAL_TYPE_MODULE;
            if (strict_module_detection && !has_suffix(filename, ".mjs")) {
                flags = JS_EVAL_TYPE_GLOBAL;
            }
            JSValue ret = TJS_EvalFile(ctx, filename, flags, true, override_filename);
            if (JS_IsException(ret)) {
                tjs_dump_error(ctx);
                JS_FreeValue(ctx, ret);
                goto fail;
            }
            JS_FreeValue(ctx, ret);
        }
        if (interactive) {
            TJS_RunRepl(ctx);
        }
        TJS_Run(qrt);
    }

    TJS_FreeRuntime(qrt);
    return 0;
fail:
    TJS_FreeRuntime(qrt);
    return 1;
}
