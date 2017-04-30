"""
**********Projet Réveille-matin**********
30 secondes avant l'alarme, il y a juste un assortiment de LED qui s'allume, des fois que je serais déjà réveillé (si pas assez fort, magasiner des led plus fortes)
À l'heure pile : tone() ou buzzer ou speaker
Si je suis willing, de quoi qui fait de la vibration à un moment donné




Actuellement (30-12-2016) : Je peux setter le temps, une alarme et avoir un signal par LED quand ca s'égalent
Actuellement (07-03-2017) : Je suis capable d'afficher des caractères sur le LCD
Actuellement (07-03-2017) : Je suis capable d'afficher l'heure de la RTC
Actuellement : Je suis capable d'afficher quelque chose sur le LCD quand il y a un interrupt de la RTC
Actuellement (29-04-2017) : Je suis capable d'utiliser un keypad avec le port série
Actuellement (29-04-2017) : Je suis capable d'afficher un input du key pad sur l'écran LCD
Prochain objectif : Pouvoir inscrire l'heure du RTC avec le keypad sans LCD (serial)

Ensuite : RTC + lCD + keypad + ABC ; 30 sec avant, allumer des led selon l'interrupt ; Produire des sons avec un speaker ; Produire des sons en lien avec l'alarme


Commandes :
* = Enter
# = cancel
C = Modifier l'heure
A = modifier l'alarme
B = Inscrire un compte à rebours
D = ??? pour l'instant