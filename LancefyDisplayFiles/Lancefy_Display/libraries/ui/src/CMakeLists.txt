SET(SOURCES screens/ui_mainScreen.c
    screens/ui_dataDisplay.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_banner_png.c
    images/ui_img_1184185672.c
    images/ui_img_1962740327.c)

add_library(ui ${SOURCES})
