    #!/bin/bash
    #usage: ./run.sh runNr
    #where runNr is a run number from runlist

    #Variablen ohne Type Deklaration
    # TestVariable=2
    # echo "testVariable ist: ${TestVariable}"
    #Substitution:  `` oder mit $(), FILELIST=`ls` -> führt das aus
    # if [TestVariable==2]; then echo "test" else echo "fail" fi, gibt auch elif=elseif


    #Argumente
    #./bin/Einkaufsliste.sh Apfel 5 Banane 8 "Frucht Korb" 15
    #  PArameter mit $1,$2,... ansprechen
    # Anzahl der Argumente: $#, String aller Argumente durch Space getrennt: $@

    #Array
    #my_array=(apfel banane obst irgendwas)
    #anderes_array[2]=apfel  -> Werte dazwischen können uninitialisiert bleiben
    #Anzahl der Elemente: ${#my_array[@]}

    #Operationen
    # Immer mit arithmetischen Ausdruck: $((ausdruck)) -> A=5 B=$((100*$A+5))
    #Potenz: a**b

    #StringOperationen
    #Length:$(#String)
    #Index eines Zeichens: expr index "$STRING" "$SUBSTRING"
    #SubString: $(STRING: $POS:$LEN) ab Position POS der Länge LEN, ohne LEN bis zum Ende
    #SubString ersetzen: ${STRING[@]/WORT/ERSETZE} ersetzt WORT durch ERSETZE beim ersten mal
    #Alle vorkommen: ${STRING[@]//WORT/ERSETZE}
    #Löscht alle vorkommen: ${STRING[@]// WORT/}

    #Vergleiche
    #String ist leer: -z "$STRING"

    #Switch
    # case "$variable" in 
    # "$BEdingung1")
    #Befel
    #;;
    #"$Bedingung2")
    #esac

    #Schleifen
    # for argument in [list] do ... done
    # while [ BEDINGUNG ] do ... done, z.B. BEDINGUNG= $COUNT -gt 0
    # break -> Bricht schleife ab, Continue-> bricht Iteration ab

    #Funktionen:
    # function_name {
    # echo §1
    #}
    # function_name "TEST"
    #

    #Spezielle VAriablen
    # $0 Filename des aktuellen Skripts

    #Files
    # File Exists: -e "$Filename" = true oder false
    # Directory Exists: -d
    # Read Permission: -r
    # grep, sort... https://www.geeksforgeeks.org/basic-shell-commands-in-linux/
    #erstellen einfach mit "Content">test.txt

    #Wichtige Commands: https://wiki.ubuntuusers.de/ls/
    # ls -la Listet Dateien in Langformat auf


    #PIPE
    #Commandos hintereinander: ls | wc -l  ->
    # Listet erst DAteien und zählt die Zeilen (wordcount - lines)

    #Allgemein
    #[[]] darin kann man Variablen ohne "$" benutzen
    #grep durchsucht Datei nach Zeichen



    #Benötigt Runlist
    #Dort jeder Eintrag: 21 21_muon6_pos5 5 -13 6 0 AB

    #Read Muss compiled sein
    #g++ geometry.C read.C analysis.C main.C `root-config --libs --cflags` -lSpectrum -o read 




    here=`pwd` #Print Working Directory Substitution
    if [ ! -d "$here/runs" ]; then
      mkdir $here/runs
    fi

    runNr=$1


    while read line
    do
      #  echo $line
        [[ $line = \#* ]] && continue    #Wenn die Zeile leer ist wird "continue" ausgeführt
        lineArr=($line)
        if [ "${lineArr[0]}" = "$runNr" ]; then   #lineArr ist ein Array aus jeder Zeile, getrennt durch Leerzeichen. linearray[0] ist die Run NUMMER
          runName=${lineArr[1]}
        printf '%s\n' "${lineArr[@]}"
          mkdir $here/runs/$runName
          if [ ! -e $here/runs/$runName/$runName.list ]; then
            ls $here/data/$runName | grep \.bin > $here/runs/$runName/$runName.list #Durchsucht das RunName verzeichnis nach bins files und erstellt eine Runlist
          fi

          counter=0
        #Für jede Bin einzeln durchlaufen
          while read line
          do
          #Create RunList for every file
           $line > $here/runs/$runName/$counter.list 
          time $here/read $here/runs/$runName/$counter.list  $here/data/$runName/ $here/runs/$runName/$counter.root ${lineArr[0]} ${lineArr[2]} ${lineArr[3]} ${lineArr[4]} ${lineArr[5]} ${lineArr[6]}
         
          counter=$((counter+1))

          done < $here/runs/$runName/$runName.list


        #  time $here/read $here/runs/$runName/$runName.list $here/data/$runName/ $here/runs/$runName/out.root ${lineArr[0]} ${lineArr[2]} ${lineArr[3]} ${lineArr[4]} ${lineArr[5]} ${lineArr[6]}
      
      
      
      
      
        fi
    done < ./runlist  #reads lines of "runslist"



    # #valgrind --trace-children=yes --tool=massif time ./../read ./$1/$1.list ./../data/$1/ ./$1/$1.root
    # #run for memory check
    # #HEAPCHECK=normal LD_PRELOAD=/usr/lib/libtcmalloc.so ./../read ./$1/$1.list ./../data/$1/ ./$1/$1.root
