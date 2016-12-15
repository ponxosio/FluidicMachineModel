COPY %1\FluidicMachineModel.dll X:\fluidicMachineModel\dll_%2\bin\FluidicMachineModel.dll
COPY %1\FluidicMachineModel.lib X:\fluidicMachineModel\dll_%2\bin\FluidicMachineModel.lib

DEL /S X:\fluidicMachineModel\dll_%2\include\*.h
XCOPY /S /Y X:\fluidicMachineModel\FluidicMachineModel\*.h X:\fluidicMachineModel\dll_%2\include