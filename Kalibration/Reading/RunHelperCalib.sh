#!/bin/bash

#rm read
# -rpath option might be necessary with some ROOT installations
# -lSpectrum option might be necessary with some ROOT installations
#g++ geometry.C read.C analysis.C main.C -rpath ${ROOTSYS}/lib `root-config --libs --cflags` -lSpectrum -o read
#g++ geometry.C read.C analysis.C main.C `root-config --libs --cflags` -lSpectrum -o read

showInformation() {
    echo "  _____             _   _    _      _                 "
    echo " |  __ \           | | | |  | |    | |                "
    echo " | |__) |___   ___ | |_| |__| | ___| |_ __   ___ _ __ "
    echo " |  _  // _ \ / _ \| __|  __  |/ _ \ | '_ \ / _ \ '__|"
    echo " | | \ \ (_) | (_) | |_| |  | |  __/ | |_) |  __/ |   "
    echo " |_|  \_\___/ \___/ \__|_|  |_|\___|_| .__/ \___|_|   "
    echo "                                     | |              "
    echo "                                     |_|              "
    echo "  _                                    _                     "
    echo " /   _. | o |_  ._ _. _|_ o  _  ._    |_  _| o _|_ o  _  ._  "
    echo " \_ (_| | | |_) | (_|  |_ | (_) | |   |_ (_| |  |_ | (_) | | "
    echo "                                                             "

    echo "This tool helps you to start your prefered ROOT reading analysis for Calibration."

    echo "Made by Jan Zimmermann in August 2019 (jan4995@gmail.com)"
    echo "-------------------------------------------------------"

}

checkDependencies() {

    PKG_OK=$(dpkg-query -W --showformat='${Status}\n' zenity | grep "ok installed")
    if [ "" == "$PKG_OK" ]; then
        echo Please enter the sudo password to install Zenity
        sudo apt-get install zenity
    else
        echo "Zenity is installed! "
    fi

    BUS_OK=$(dpkg-query -W --showformat='${Status}\n' dbus-x11 | grep "ok installed")
    if [ "" == "$BUS_OK" ]; then
        echo Please enter the sudo password to install DBUSx11
        sudo apt-get install dbus-x11
    else
        echo "dbus-x11 is installed! "
    fi
}

chooseOutFolder() {
    outFolder=$(zenity --file-selection --directory --title "Select outpu Folder (.bin Files)?")
}

chooseInFolder() {
    inFolder=$(zenity --file-selection --directory --title "Select input Folder (.bin Files)?")
}
changeFolders() {
    echo "Which folder do you want to change?"
    select yn in "Input" "Output"; do
        case $yn in
        Input)
            chooseInFolder
            break
            ;;
        Output) chooseOutFolder ;;
        esac
    done

}
createInAndOutFolder() {
    if [ ! -d "$1" ]; then
        mkdir "$1"
    fi
    if [ ! -d "$2" ]; then
        mkdir "$2"
    fi
}

compileReadFast() {
    echo "Compiling ReadFast..."
  #  g++ geometry.C readFast.C analysis.C mainFast.C $(root-config --libs --cflags) -lSpectrum -o readFast
    echo "Compiling done!"
}

compileReadFull() {
    echo "Compiling ReadFull..."
    g++ geometry.C readFull.C analysis.C mainFull.C $(root-config --libs --cflags) -lSpectrum -o readFull
    echo "Compiling done!"
}
readFast() {

    compileMerger

    runNr=$1
    readAll=false
    inFolder=$2
    outFolder=$3
    saveFolder=$outFolder/Fast/
    deleteParts=false
    useCalibValues="0"
    dynamicBL="0"
    isDC="0"
    echo "Is this a dark count measurement? (different time integration)"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            isDC="1"
            break
            ;;
        No) isDC="0" break ;;
        esac
    done

    echo "Do you want to use dynamic Baselines?"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            dynamicBL="1"
            break
            ;;
        No) dynamicBL="0" break ;;
        esac
    done
    echo "Are the calibration values already correct"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            useCalibValues="1"
            break
            ;;
        No)
            useCalibValues="0"
            break
            ;;
        esac
    done
    echo "You want to delete the part files afterwards?"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            deleteParts=true
            break
            ;;
        No)
            deleteParts=false
            break
            ;;
        esac
    done

    echo $dynamicBL

    if [ "$runNr" = "a" ]; then
        readAll=true
    fi
    mkdir "$saveFolder"
    rootFileList=""

    time (
        for folder in $inFolder/*; do
            [[ $folder == \#* ]] && continue
            voltageNumber="${folder//[!0-9]/}"
            IFS='/'                    # / is set as delimiter
            read -ra ADDR <<<"$folder" # folder is read into an array as tokens separated by IFS
            IFS=""
            runName=${ADDR[-1]} #last element
            runDir=$saveFolder/$runName

            if [ "${voltageNumber}" = "$runNr" ] || [ $readAll = true ]; then
                mkdir "$runDir"
                if [ ! -e runDir/$runName.list ]; then
                    ls $inFolder/$runName | grep \.bin >$runDir/$runName.list
                fi

                counter=0
                #Für jede Bin einzeln durchlaufen
                while read line; do
                    #Create RunList for every file
                    echo $line >$runDir/$counter.list
                    mkdir $runDir/$counter

                    $here/readFast $runDir/$counter.list $inFolder/$runName/ $runDir/$counter $runName $dynamicBL $isDC $useCalibValues

                    runDirRelative="${runDir//$here/}"
                    rootTreeFilePath=".$runDirRelative/$counter/out.root/T"
                   
                    rootFileList="${rootFileList}||$rootTreeFilePath"



                    echo "$rootFileList"

                    counter=$((counter + 1))

                done <$runDir/$runName.list

            fi
            merger $rootFileList ".$runDirRelative/$runName.root"

            echo "Deleting: $runDir"

            if [ $deleteParts = true ]; then
                rm -rf $runDir/*/
                find $runDir -name "*.list" -type f -delete

            fi

        done

    )

    #reads lines of "runslist"

}

readFull() {
    runNr=$1
    readAll=false
    inFolder=$2
    outFolder=$3
    saveFolder=$outFolder/Full/
    useCalibValues="0"
    dynamicBL="0"
    isDC="0"
    echo "Is this a dark count measurement? (different time integration)"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            isDC="1"
            break
            ;;
        No) isDC="0" break ;;
        esac
    done

    echo "Do you want to use dynamic Baselines?"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            dynamicBL="1"
            break
            ;;
        No) dynamicBL="0" break ;;
        esac
    done
    echo "Are the baselines values already correct"
    select yn in "Yes" "No"; do
        case $yn in
        Yes)
            useCalibValues="1"
            break
            ;;
        No)
            useCalibValues="0"
            break
            ;;
        esac
    done

    echo $dynamicBL

    if [ "$runNr" = "a" ]; then
        readAll=true
    fi
    mkdir "$saveFolder"

    for folder in $inFolder/*; do
        [[ $folder == \#* ]] && continue
        voltageNumber="${folder//[!0-9]/}"
        IFS='/'                    # / is set as delimiter
        read -ra ADDR <<<"$folder" # folder is read into an array as tokens separated by IFS
        IFS=""
        runName=${ADDR[-1]} #last element
        runDir=$saveFolder/$runName

        if [ "${voltageNumber}" = "$runNr" ] || [ $readAll = true ]; then
            mkdir "$runDir"
            if [ ! -e runDir/$runName.list ]; then
                ls $inFolder/$runName | grep \.bin >$runDir/$runName.list
            fi
            time $here/readFull $runDir/$runName.list $inFolder/$runName/ $runDir/$runName.root $runName $dynamicBL $isDC $useCalibValues
        fi
    done

}

compileMerger() {
    echo "Compiling Root Merger..."
    g++ mergeROOTFiles.C $(root-config --libs --cflags) -lSpectrum -o mergeROOTFiles
    echo "Compiling done!"
}

merger() {
    rootFileList=$1
    $here/mergeROOTFiles $rootFileList $2 $3
}

readRoot() {
    readMode=$1  #0= FULL #1=FAST
    runNumber=$2 #a=ALL
    inFolder=$3
    outFolder=$4

    case $readMode in
    0)
        readFull $runNumber $inFolder $outFolder
        ;;
    1)
        readFast $runNumber $inFolder $outFolder
        break
        ;;
    esac

}

# ██████╗ ██████╗ ██████╗ ███████╗
#██╔════╝██╔═══██╗██╔══██╗██╔════╝
#██║     ██║   ██║██║  ██║█████╗
#██║     ██║   ██║██║  ██║██╔══╝
#╚██████╗╚██████╔╝██████╔╝███████╗
# ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝

showInformation
checkDependencies

here=$(pwd) #Print Working Directory Substitution

inFolder=$here/data
outFolder=$here/runs

fastGo=false
echo "-------------------------------------------------------"
echo "Input data folder: $inFolder"
echo "Output data folder: $outFolder"
echo "Want to use this file locations?"
select yn in "Yes" "No"; do
    case $yn in
    Yes)
        createInAndOutFolder $inFolder $outFolder
        fastGo=true

        break
        ;;
    No) changeFolders break ;;
    esac
done

if [[ $fastGo != true ]]; then
    echo "---------------------------------------------"
    echo "Can we continue with the following Folders?:"
    echo "Input data folder: $inFolder"
    echo "Output data folder: $outFolder"
    select yn in "Yes" "No"; do
        case $yn in
        No)
            changeFolders
            break
            ;;
        Yes) break ;;
        esac
    done
fi
echo "---------------------------------------------"
echo "Should the read files be compiled?"
select yn in "Yes" "No"; do
    case $yn in
    Yes)
        compileReadFast
        compileReadFull

        break
        ;;
    No) break ;;
    esac
done

echo "---------------------------------------------"
echo "Choose a read mode: "
echo "Full: High memory usage but all histogramms (1 Root file, fast at small datasets)"
echo "Fast: Low memory usage but broken histogramms (Multiple merged Root file, slow at small datasets)"

readMode=0
select yn in "Full" "Fast"; do
    case $yn in
    Fast)
        readMode=1
        break

        ;;
    Full)
        readMode=0
        break
        ;;
    esac
done
echo "---------------------------------------------"

echo "Choose a Calib Voltage number (ALL=a, Example: 58 (=58 Bias Voltage))"

runNumber=a
read runNumber

readRoot $readMode $runNumber $inFolder $outFolder

echo "---------------------------------------------"
echo "  ____    ___   _   _  _____ "
echo " |  _ \  / _ \ | \ | || ____|"
echo " | | | || | | ||  \| ||  _|  "
echo " | |_| || |_| || |\  || |___ "
echo " |____/  \___/ |_| \_||_____|"
echo "                             "
echo "---------------------------------------------"

# sudo apt-get install zenity
#FILE=$(dialog --title "Delete a file" --stdout --title "Please choose a file to delete" --fselect /tmp/ 14 48)
