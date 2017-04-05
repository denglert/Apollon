export EVT_ANALYSIS=Zh-PartonLevel
export MAIN_ANALYSIS=RunAnalysis
export WORKDIR='/home/de3u14/lib/projects/Apollon/tmp/test/'
export BINCONFIG=./config/bin_test.conf
export COMPCONFIG=./config/comp_test.conf
export nEvents=-1

make build EVT_ANALYSIS=${EVT_ANALYSIS} MAIN_ANALYSIS=${MAIN_ANALYSIS}
#./scripts/createWD.sh

echo ""
echo "-------------------------"
echo "Analysis:   ${ANALYSIS}"
echo "WORKDIR:    ${WORKDIR}"
echo "BINCONFIG:  ${BINCONFIG}"
echo "COMPCONFIG: ${COMPCONFIG}"
echo "nEvents:    ${nEvents}"
echo "-------------------------"
echo ""

./bin/${MAIN_ANALYSIS}-${EVT_ANALYSIS} ${WORKDIR} ${COMPCONFIG} ${BINCONFIG} ${nEvents}
