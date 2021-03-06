# Projekt-Omega

## Inhaltsverzeichnis
* [Kurzbeschreibung](#kurzbeschreibung)
* [Hardware](#hardware)
* [Software](#software)
  * [Indoor](#indoor)
  * [Outdoor](#outdoor)
  * [APP](#app)
  * [Node-RED](#node-red)
 
## Kurzbeschreibung
Dies ist "Projekt-Omega" - ein Smart-Home Projekt, welches für das Modul Soft Skills und Technische Kompetenzen als Prüfungsleistung entstanden ist. Die Aufgabe war es, ein Gerät zu entwickeln, das den Alltag angenehmer gestaltet oder ein konkretes Problem löst. Als weitere Bedingung musste mindestens ein Teil des Gadgets aus dem 3D-Drucker oder dem Lasercutter kommen. 

Das Ergebnis besteht aus zwei Prototypen: das Indoor-Haus und das Outdoor-Haus.
![Prototyp-Drinnen](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/home_v1_1-1024x859.jpg)
![Prototyp-Draußen](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/small_home_v1_1-1024x904.jpg)


## Hardware
Indoor-Haus:
* WEMOS D1 mini
* OLED Display
* DC Power Shield
* Rotary Encoder
* DHT Shield
* MQ-3 Sensor
* Kipp-Schalter
* Buzzer Shield
* Tripler Base

Outdoor-Haus:
* WEMOS D1 mini
* Battery Shield

Im Folgenden sind die Schaltpläne der beiden Häuser dargestellt.
![Schaltplan-Indoor](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/Schematic_Normales_Haus-1024x700.png)
![Schaltplan-Outdoor](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/Schematics_miniHaus-1024x494.png)

## Software
# Indoor
![Software-Indoor](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/Software-Indoor-Haus-1024x372.png)

Die Software für das Indoor Haus ist einfach zusammenfassen, bietet jedoch eine gewisse Komplexität. Wurde die "requestData"-Flag gesetzt, so wird über MQTT die Node-RED Instanz angestoßen. Diese fordert dann bei der entsprechenden API die benötigten Daten an. Diese Daten werden an das Haus zurückgeschickt und können in der "handle MQTT" Phase verarbeitet werden. 

Nach der Verarbeitung sämtlicher MQTT Nachrichten wird geprüft, ob noch weitere Anfragen verarbeitet werden müssen oder ob der Bildschirm deaktiviert ist. Ist dies der Fall, werden die Displays geleert und die Schleife springt zum Anfang zurück. Die Variable "lightOn" kann mittels MQTT, also der App, und per Interrupt an dem Schalter vorne am Haus verändert werden.

Sind die Displays hingegen aktiviert und sollen Daten anzeigen, wird auf einem Display ein Auswahlmenü eingeblendet und auf dem anderen Display werden die Daten angezeigt. Mithilfe des Drehimpulsgebers (Schornstein) ist das Menü navigierbar. Auch diese Interaktion wird über Interrupt und die App realisiert, sodass nicht immer in der Loop erfragt werden muss, ob eine Auswahl stattgefunden hat.

# Outdoor

![Software-Outdoor](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/Software-Outdoor-Haus-1024x285.png)

Das Outdoor-Haus erlaubt keine Interaktion und ist so implementiert, dass es von überall Daten aufnehmen und an das Indoor-Haus senden kann. Dazu verbindet sich der Mikrocontroller zuerst mit dem WiFi und baut eine Verbindung zum MQTT-Broker auf. Daraufhin folgt eine Aufnahmereihe von 10 Aufnahmen, aus welchen ein Mittelwert errechnet wird. Ist dies geschehen, werden die Daten mittels MQTT an alle abonnierenden MQTT-Teilnehmer gesendet (Indoor-Haus). Abschließend geht der Controller für eine Minute in den Deep Sleep Modus. Um aus diesem Modus wieder "erwachen" zu können, müssen der D0 und der RST Pin miteinander verbunden sein. Die Nutzung des Deep Sleep Modus hat zwei verschiedene Gründe:

* Der Wemos d1 Mini produziert im Dauerbetrieb eine enorme Menge an Wärme, welche unweigerlich die Aufnahme der Temperaturwerte verfälscht. In einigen Tests wurden Temperaturabweichungen bis zu 10 °C festgestellt.
* Da der Wemos eine Minute lang nahezu ausgeschaltet ist, wird dabei keine Energie verbraucht und der Akku muss seltener geladen werden.


## Bibliotheken
Um das Projekt realisieren zu können, wurden diverse Bibliotheken verwendet. Diese Bibliotheken können alle im Library Manger der Arduino IDE heruntergeladen werden:


- "Adafruit GFX Library" by Adafruit, v1.10.10
- "Adafruit SSD1306 Wemos Mini OLED" by Adafruit + mcauser, v1.1.2
- "ArduinoMqttClient" by Arduino, v0.1.5
- "WiFiManager" by tzapu,tablatronix, v2.0.3-alpha
- "DHT sensor library" by Adafruit, v1.3.9
- "Time" by Michael Margolis, v1.6.1 (https://github.com/PaulStoffregen/Time)



<p>Da die Bibliothek "Adafruit SSD1306 Wemos Mini OLED" die selben Klassennamen hat wie die Bibliothek "Adafruit SSD1306" by Adafruit, besteht die Gefahr, dass der Compiler nicht die richtige Klasse zuordnen kann. Um dieses Problem zu umgehen, wurden die Klassen in der Bibliothek "Adafruit SSD1306 Wemos Mini OLED" mittels find and replace so bearbeitet, dass alle Klassennamen um den Suffix "_WEMOS" ergänzt wurden. Auf diese Weise können beide Bibliotheken simultan verwendet werden.


# APP

Die Implementierung der App erfolgte online mit Hilfe der Website MIT App Inventor. Durch die Erweiterung eines Beispielprogramms, welches bereits MQTT implementiert hatte, konnte relativ simpel eine einfache App gestaltet und mit MQTT-Funktionalität ausgestattet werden.

Blynk kam in diesem Projekt nicht mehr zum Einsatz. Grund dafür ist Blynks neuer Businessplan, der nicht mit Zielen des Projekts verainbar ist.

# Node-RED

![Node-RED-flow](https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20202021-sosepg-13/wp-content/uploads/sites/7451/2021/09/Node-RED-flow-1024x211.png)

Der Node-RED-Flow ist aktuell darauf begrenzt, Anfragen an die SpaceX-API zu triggern und die Daten dann wieder mittels MQTT zu übermitteln.
















