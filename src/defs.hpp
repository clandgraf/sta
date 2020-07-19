#pragma once

#ifndef DEFS_HPP
#define DEFS_HPP

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__));
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__; __pragma( pack(pop))
#endif

#define APP_NAME "staNES"
#define VERSION "0.1.0"

#define WINDOW_TITLE (APP_NAME VERSION)

#endif
