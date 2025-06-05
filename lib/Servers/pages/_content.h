#pragma once
#ifndef __content__
#define __content__
#include "Arduino.h"
#include "config_p.h"
#include "home_p.h"
#include "style_s.h"
#include "update_p.h"

typedef struct{
    const char * name ;
    const uint8_t * address;
    size_t size;
    const char * type;
}_content_t;

_content_t __config_temp = {
    .name = "config",
    .address = CONFIG_PAGE,
    .size = sizeof(CONFIG_PAGE)
,    .type = "text/html"
};
_content_t __home_temp = {
    .name = "home",
    .address = HOME_PAGE,
    .size = sizeof(HOME_PAGE)
,    .type = "text/html"
};
_content_t __style_temp = {
    .name = "style",
    .address = STYLE_STYLE,
    .size = sizeof(STYLE_STYLE)
,    .type = "text/css"
};
_content_t __update_temp = {
    .name = "update",
    .address = UPDATE_PAGE,
    .size = sizeof(UPDATE_PAGE)
,    .type = "text/html"
};

_content_t * __web_contents[]={ &__config_temp, &__home_temp, &__style_temp, &__update_temp };

#endif
