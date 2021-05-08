stack_top_symb  := :
stack_separator := %

cat = $(firstword $(1))$(if $(filter-out $(firstword $(1)),$(1)),$(2)$(call cat,$(filter-out $(firstword $(1)),$(1)),$(2)))

# $(call stack-top-raw, STACK_NAME)
define stack-top-raw
$(subst $(stack_top_symb),,$(firstword $($(1))))
endef

# $(call stack-top, STACK_NAME)
define stack-top
$(subst $(stack_separator), ,$(call stack-top-raw,$(1)))
endef

# $(call stack-push, STACK_NAME, VALUE)
define stack-push
$(eval $(1) := $(stack_top_symb)$(strip $(call cat,$(2),$(stack_separator))) $(subst $(stack_top_symb),,$($(1))))
endef

# $(call stack-pop, STACK_NAME)
define stack-pop
$(eval __top := $(call stack-top-raw,$(1)))
$(eval $(1)  := $(stack_top_symb)$(subst $(stack_top_symb)$(__top) ,,$($(1))))
endef
