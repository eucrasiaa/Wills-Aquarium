#pragma once
//
// static const char *anim_fish_f1 = " <X>< ";
// static const char *anim_fish_f2 = " <X>  ";
// static const char *anim_fish_f3 = " <X>- ";
#define ANIM_FISH_F1 " <X>< "
#define ANIM_FISH_F2 " <X>  "
#define ANIM_FISH_F3 " <X>- "


// static const char **anim_fish_anim[] = { &anim_fish_f1, &anim_fish_f2, &anim_fish_f3 };
static const char *anim_fish_anim[] = { ANIM_FISH_F1, ANIM_FISH_F2, ANIM_FISH_F3 };

#define BASIC_FISH_F1 " <)>< "
// static const char *basic_fish_f1 = " <)>< ";
// static const char *basic_fish_anim[] = { basic_fish_f1 };
static const char *basic_fish_anim[] = { BASIC_FISH_F1 };

//  /  /\\/ \\/\\  \\   

#define WIDE_FISH_F1 "  /  /\\/ \\/\\  \\   "
static const char *wide_fish_anim[] = { WIDE_FISH_F1 };
