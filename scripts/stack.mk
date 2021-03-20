stack_top_symb := :

# $(call stack-top, STACK_NAME)
define stack-top
$(subst $(stack_top_symb),,$(firstword $($(1))))
endef

# $(call stack-push, STACK_NAME, VALUE)
define stack-push
$(eval $(1) := $(stack_top_symb)$(strip $(2)) $(subst $(stack_top_symb),,$($(1))))
endef

# $(call stack-pop, STACK_NAME)
define stack-pop
$(eval __top := $(call stack-top,$(1)))
$(eval $(1)  := $(stack_top_symb)$(subst $(stack_top_symb)$(__top) ,,$($(1))))
endef
