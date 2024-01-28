Projet Simon Game
Description

Ce projet consiste en la création du jeu le Simon, mettant en œuvre une interface utilisateur composée d'un clavier et d'une série de LEDs. Le jeu suit une séquence de LEDs qui s'allument de manière aléatoire, puis demande au joueur de reproduire cette séquence en appuyant sur les touches correspondantes du clavier.

Matériel requis

    Arduino (compatible)
    Écran LCD I2C
    Clavier matriciel 4x4 HC 35 avec  LEDs (4)
    Buzzer

Bibliothèques nécessaires

    LiquidCrystal_I2C.h (pilote l'écran LCD via le protocole I2C)
    Key.h (fonctionnalités du clavier)
    Keypad.h (pilote le clavier matriciel)
    obj_stat.h (bibliothèque locale de programmation de la machine à états)

Connexions matérielles

    Clavier matriciel HC 35: Connecter les broches ROWS et COLS aux broches correspondantes de l'Arduino.
    Buzzer : Connecter le buzzer à la broche A0 de l'Arduino.

Configuration

    Ajuster les constantes ROWS, COLS, rowPins, et colPins selon votre configuration matérielle.
    Assurez-vous d'installer les bibliothèques LiquidCrystal_I2C, Key, et Keypad dans votre environnement de développement Arduino.

Fonctionnement

    Lorsque le jeu démarre, une mélodie est jouée, et le niveau initial est affiché sur l'écran LCD.
    Une séquence de LEDs s'allume aléatoirement, puis le joueur doit reproduire la séquence en appuyant sur les touches correspondantes du clavier.
    Si le joueur réussit, un signal sonore indique la réussite, le niveau est incrémenté, et une nouvelle séquence est affichée.
    En cas d'erreur, une animation lumineuse et un message d'erreur sont affichés, et le jeu revient à l'écran d'accueil.
