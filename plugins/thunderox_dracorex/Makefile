
# --------------------------------------------------------------
# Project name, used for binaries

NAME = thunderox_dracorex

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	dracorexPlugin.cpp \
	oscillator.cpp \
	voice.cpp \
	chorus.cpp \

FILES_UI  = \
	../../delirium_ui/delirium_ui.cpp \
	../../delirium_ui/delirium_ui_widget_base.cpp \
	../../delirium_ui/delirium_ui_widget_label.cpp \
	../../delirium_ui/delirium_ui_widget_fader.cpp \
	../../delirium_ui/delirium_ui_widget_switch.cpp \
	../../delirium_ui/delirium_ui_widget_logo.cpp \
	../../delirium_ui/delirium_ui_widget_knob.cpp \
	../../delirium_ui/delirium_ui_widget_button.cpp \
	../../delirium_ui/delirium_ui_widget_panel.cpp \
	../../delirium_ui/delirium_ui_widget_adsr.cpp \
	../../delirium_ui/delirium_ui_widget_selector.cpp \
	../../delirium_ui/delirium_ui_widget_filter.cpp \
	../../delirium_ui/delirium_ui_widget_fader_route.cpp \
	../../delirium_ui/delirium_ui_widget_list.cpp \
	../../delirium_ui/delirium_ui_widget_tabbed_navigator.cpp \
	dracorexUI.cpp

# --------------------------------------------------------------
# Do some magic

UI_TYPE = cairo
include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
ifeq ($(HAVE_CAIRO),true)
TARGETS += jack
endif
endif

ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_CAIRO),true)
ifeq ($(HAVE_LIBLO),true)
TARGETS += dssi
endif
endif
endif

ifeq ($(HAVE_CAIRO),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
