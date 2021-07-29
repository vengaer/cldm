ifneq ($(ARCH),)
  CROSS_LDFLAGS := -L /usr/$(ARCH)-linux-gnu
  CROSS_EMU     := qemu-$(ARCH) $(CROSS_LDFLAGS)
  CROSS_COMPILE := $(ARCH)-linux-gnu-
endif
