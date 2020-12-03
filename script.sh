#!/bin/bash

ARCHIVO='programa' 

function mostrarPrograma()
{
	cat $ARCHIVO.c
}

function compilarPrograma()
{
	gcc -o $ARCHIVO $ARCHIVO.c

}

function ejecutarPrograma()
{
	if(test -f $ARCHIVO)
	then
		if(test -x $ARCHIVO)
		then
			
			echo Introduce el numero de pacientes a tratar
			read argument
			./$ARCHIVO $argument
		else
			echo El archivo no es ejecutable
		fi
	else
		echo El archivo no existe o no ha sido compilado
	fi
}

while true
do
	
	echo ________________________________________________             
	echo                       MENU
	echo ________________________________________________
	echo
	echo 1. Mostrar codigo de programa
	echo
	echo 2. Compilar codigo de programa
	echo
	echo 3. Ejecutar programa
	echo
	echo 4. Salir
	echo

	echo Elige una de las opciones anteriores:
	read input

	case $input in
		1) echo`mostrarPrograma`;;
		2) echo`compilarPrograma`;;
		3) echo`ejecutarPrograma`;;
		4) exit 0;;
		*) echo No has elegido una opcion correcta;;
	esac
done
