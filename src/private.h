/*
 * QuickJS libuv bindings
 *
 * Copyright (c) 2019-present Saúl Ibarra Corretgé <s@saghul.net>
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

#ifndef TJS_PRIVATE_H
#define TJS_PRIVATE_H

#include <cutils.h>

#include <quickjs.h>
#include <stdbool.h>
#include <uv.h>

#ifdef TJS_HAVE_CURL
#include <curl/curl.h>
#endif

// TODO: improve this.
#if defined(_WIN32)
#define TJS__PLATFORM "win32"
#elif defined(__APPLE__)
#define TJS__PLATFORM "darwin"
#elif defined(__linux__)
#define TJS__PLATFORM "linux"
#else
#define TJS__PLATFORM "posix"
#endif


struct TJSRuntime {
    JSRuntime *rt;
    JSContext *ctx;
    uv_loop_t loop;
    struct {
        uv_check_t check;
        uv_idle_t idle;
        uv_prepare_t prepare;
    } jobs;
    uv_async_t stop;
    bool is_worker;
#ifdef TJS_HAVE_CURL
    struct {
        CURLM *curlm_h;
        uv_timer_t timer;
    } curl_ctx;
#endif
};

void tjs_mod_dns_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_dns_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_error_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_error_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_fs_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_fs_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_misc_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_misc_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_process_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_process_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_signals_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_signals_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_std_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_std_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_streams_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_streams_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_timers_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_timers_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_udp_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_udp_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_worker_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_worker_export(JSContext *ctx, JSModuleDef *m);
void tjs_mod_xhr_init(JSContext *ctx, JSModuleDef *m);
void tjs_mod_xhr_export(JSContext *ctx, JSModuleDef *m);

JSValue tjs_new_error(JSContext *ctx, int err);
JSValue tjs_throw_errno(JSContext *ctx, int err);

JSValue tjs_new_pipe(JSContext *ctx);
uv_stream_t *tjs_pipe_get_stream(JSContext *ctx, JSValueConst obj);

int tjs__load_file(JSContext *ctx, DynBuf *dbuf, const char *filename);
JSModuleDef *tjs_module_loader(JSContext *ctx, const char *module_name, void *opaque);
char *tjs_module_normalizer(JSContext *ctx, const char *base_name, const char *name, void *opaque);

JSModuleDef *js_init_module_std(JSContext *ctx, const char *module_name);
int js_module_set_import_meta(JSContext *ctx, JSValueConst func_val, JS_BOOL use_realpath, JS_BOOL is_main);

JSValue tjs__get_args(JSContext *ctx);

int tjs__eval_binary(JSContext *ctx, const uint8_t *buf, size_t buf_len);
void tjs__bootstrap_globals(JSContext *ctx);
void tjs__add_builtins(JSContext *ctx);

#endif
