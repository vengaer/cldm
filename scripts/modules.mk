include scripts/stack.mk

module_stack   :=
trivial_stack  :=
required_stack :=

# $(call mk-module-build_dir)
define mk-module-build-dir
$(shell $(MKDIR) $(MKDIRFLAGS) $(patsubst $(root)/%,$(builddir)/%,$(module_path)))
endef

# $(call include-module-prologue, MODULE_NAME)
define include-module-prologue
$(if $(DEBUG),$(info include-module-prologue $(1)))

$(eval module_name := $(strip $(1)))

$(call stack-push,module_stack,$(module_name))
$(call stack-push,trivial_stack,$(trivial_module))
$(call stack-push,required_stack,$(required_by))

$(eval module_path := $(module_path)/$(module_name))

$(call mk-module-build-dir)

$(eval trivial_module := n)
$(eval required_by := )
endef

# $(call include-module-epilogue)
define include-module-epilogue
$(if $(DEBUG),$(info include-module-epilogue $(module_name)))

$(if $(findstring y,$(trivial_module)),
    $(call declare-trivial-c-module))

$(eval trivial_module := $(call stack-top,trivial_stack))
$(eval required_by := $(call stack-top,required_stack))
$(eval module_path := $(patsubst %/$(module_name),%,$(module_path)))

$(call stack-pop,module_stack)
$(call stack-pop,trivial_stack)
$(call stack-pop,required_stack)

$(eval module_name := $(call stack-top,module_stack))
endef

# $(call include-module,MODULE_NAME)
define include-module
$(call include-module-prologue,$(1))
$(eval include $(module_path)/$(module_mk))
$(call include-module-epilogue)
endef

# $(call declare-trivial-c-module)
define declare-trivial-c-module
$(if $(DEBUG),$(info declare-trivial-c-module $(module_name)))

$(eval __src := $(wildcard $(module_path)/*.$(cext)))
$(eval __obj := $(patsubst $(root)/%.$(cext),$(builddir)/%.$(oext),$(__src)))

$(foreach __t,$(required_by),
    $(eval
        $(__t)_obj := $(__obj)))

$(eval CPPFLAGS += -I$(module_path))
endef

# $(call include-each-module,MODULE_NAME...)
define include-each-module
$(foreach __m,$(1),$(call include-module,$(notdir $(__m))))
endef
