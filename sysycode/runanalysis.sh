#!/bin/bash

ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
echo "Runing lxplus initscript"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
localSetupROOT
rcSetup

#DAOD_TRUTH1.m100_DeltaM9_C1C1.root
#DAOD_TRUTH1.m100_DeltaM9_N2C1.root
#DAOD_TRUTH1.TauTauJets_rapidityOrderOff.root
#DAOD_TRUTH1.ttbarJets.root
#DAOD_TRUTH1.Wjets_rapidityOrderOff.root
#DAOD_TRUTH1.WWJets.root

pkill tmux

tmux new-session -d -s calc # -n runstuff
tmux new-window -t calc:1 -n m100_DeltaM9_C1C1
tmux new-window -t calc:2 -n m100_DeltaM9_N2C1
tmux new-window -t calc:3 -n TauTauJets
tmux new-window -t calc:4 -n ttbarJets
tmux new-window -t calc:5 -n WJets
tmux new-window -t calc:6 -n WWJets

# #tmux new-window -t my_server:1 -n someotherjunk

# tmux send-keys -t calc:1 "runAna m100_DeltaM9_C1C1" C-m
# tmux send-keys -t calc:2 "runAna m100_DeltaM9_N2C1" C-m
# tmux send-keys -t calc:3 "runAna TauTauJets_rapidityOrderOff" C-m
# tmux send-keys -t calc:4 "runAna ttbarJets" C-m
# tmux send-keys -t calc:5 "runAna Wjets_rapidityOrderOff" C-m
# tmux send-keys -t calc:6 "runAna WWJets" C-m

tmux send-keys -t calc:1 "runAna m100_DeltaM9_C1C1.v2.1" C-m
tmux send-keys -t calc:2 "runAna m100_DeltaM9_N2C1.v2.1" C-m
tmux send-keys -t calc:3 "runAna TauTauJets.v2.0" C-m
tmux send-keys -t calc:4 "runAna ttbarJets.v1.0" C-m
tmux send-keys -t calc:5 "runAna WJets.v1.0" C-m
tmux send-keys -t calc:6 "runAna WWJets.v1.0" C-m

#tmux select-window -t my_server:runstuff
#tmux
tmux attach-session -t calc
