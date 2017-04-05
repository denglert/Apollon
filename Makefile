# - Test
#RUN_EVT_ANALYSIS   = Zh-PartonLevel
RUN_EVT_ANALYSIS   = Zh-PartonLevel-with-cuts
RUN_MAIN_ANALYSIS  = RunAnalysis
RUN_ANA_TAG        = test
RUN_ANA_BINCONFIG  = ./config/bin_test.conf
RUN_ANA_COMPCONFIG = ./config/comp_test.conf
RUN_ANA_nEvents    = -1

VAR_RUN_ANA    = $(shell echo '$(.VARIABLES)' |  awk -v RS=' ' '/RUN_ANA_/' | sed 's/RUN_ANA_//g' )
EXPORT_RUN_ANA = $(foreach v,$(VAR_RUN_ANA),$(v)="$(RUN_ANA_$(v))")

all : build

run: build
	@$(EXPORT_RUN_ANA) ./scripts/createWD.sh
	@./bin/$(RUN_MAIN_ANALYSIS)-$(RUN_EVT_ANALYSIS) $(RUN_ANA_TAG) $(RUN_ANA_COMPCONFIG) $(RUN_ANA_BINCONFIG) $(RUN_ANA_nEvents)

display_vars :
	@ echo "VAR_RUN_ANA:    $(VAR_RUN_ANA)"
	@ echo "EXPORT_RUN_ANA: $(EXPORT_RUN_ANA)"

###############################

build : 
	cd src; EVT_ANALYSIS=$(RUN_EVT_ANALYSIS) MAIN_ANALYSIS=$(RUN_MAIN_ANALYSIS) make all;

export $(EVT_ANALYSIS)
export $(MAIN_ANALYSIS)

test :
	@./RunAnalysis_test.sh

build_manual : 
	@if [ -z $(EVT_ANALYSIS) ] || [ -z $(MAIN_ANALYSIS) ]; then\
		echo "EVT_ANALYSIS  or MAIN_ANALYSIS variable not defined";\
		echo "Please specify which analysis you want to compile.";\
		echo "e.g. make build_manual ANALYSIS=Zh-PartonLevel";\
	 else\
	 cd src; make all; \
	 fi
	
clean :
	rm -f ./lib/*
	rm -f ./bin/*
