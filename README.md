# Arduino Security System
A security system created using an Arduino and other components as well as an express API.

## Usage
To be used correctly, the information for the wifi and an email must be added to the ESP file. The location of the Express API must also be added. If the product is local then this will be the devices ip address. Finally in the API app.js, the email to send the email from's address and password must be added.
## Libraries Used
- ESP8266WiFi – Arduino Public Library
- ESP8266HTTPClient - Arduino Public Library
-	WifiClient - Arduino Public Library
-	LiquidCrystal - Class
-	Keypad – GNU Public License

## Components
-	LCD1602 Module – Digital Out: LCD Module is used as the display output for the security system. It says whether the system is disarmed or armed as well as showing the results of inputs into the board and other messages that are needed to pass information to the user.
-	Membrane Switch Module - Digital Out: Membrane Switch Module is used for user input into the system. The user can enter their security code and use the A, C, and D to arm, change the password or disarm the system.  
-	HC-SR501 PIR Motion Sensor Module – Digital Out: Motion Sensor Module is used as the main sensor of the system. When armed it waits for motion and if motion is sensed it sets off the security system. 
-	Piezo Buzzer – Digital Out: Piezo Buzzer is used to signal whether the sensor was set off. The buzzer emits a low frequency buzz for a time to signal a grace period where the password can be put in to stop the system. After the grace period the buzzer switches to a higher frequency to signal the grace period is over.
-	ESP8266 Module – UART: ESP Module is used so the Arduino board can communicate with the express API. After the grace period ends the Arduino board will write to serial and this will cause the ESP Module to send an alert to the Express API.
-	Express API – NodeJS: Express API is used to send an email alert once the system senses motion and the grace period ends. The has a post request that is called with an email passed and it sends the time of the call to the email address.


## Specifications
The security system begins in disarmed mode with a default security code and a master code. While disarmed, the security code can be inputted and either ‘A’ or ‘C’ be pressed. If ‘A’ is pressed the system becomes armed. If ‘C’ is pressed the system asks for a new code to be inputted. Once it is typed in it asks for it to be typed in again. If both codes match, then the security code is changed. Otherwise, it is not, and an error message appears saying so. While the system is armed, it waits to see if any motion is sensed by the motion detector. At any time, the security code can be entered and when ‘D’ is clicked after the system is disarmed. When motion is detected, the buzzer begins emitting a low frequency noise for an allotted time, the grace period. During this time the system can be disarmed as usual with the security code and if it does nothing else happens. If the grace period ends, an email is sent to the saved email for the user with the date and time of the grace period ending. During the grace period, the buzzer also switches to a high pitch frequency and the master code must be entered after the normal disarming process to disarm the system. At any point, an invalid choice shows a message saying so. Any time when input is begun, a timeout period exists where if the input does not end during this time, the system resets to before the input started. The security code and master code are both 4 characters long.

## Context Diagram
![Context Diagram](https://github.com/Ominsi/Arduino_Security_System/blob/main/Diagrams/ContextDiagram.png?raw=true)
## Sequence Diagram
![Sequence Diagram](https://github.com/Ominsi/Arduino_Security_System/blob/main/Diagrams/SequenceDiagram.png?raw=true)
## Wiring Diagram
![Wiring Diagram](https://github.com/Ominsi/Arduino_Security_System/blob/main/Diagrams/WiringDiagram.png?raw=true)
## Transition Functions
### Arduino to ESP
- Serial.write() -> Writes to the serial, allowing to be read from the ESP.
-	Serial.available() -> Checks whether a message is in the Serial.
-	Serial.read() -> Reads the message from the Serial.
### ESP to Express API
-	http.POST() -> Sends a post request to the specified address
-	app.post() (NodeJS) -> Receives the post request.
-	res.send() (NodeJS) -> Sends a response to the ESP with whether the email was a success or not. 

##I/O Functions
### LCD Screen
- lcd.clear() -> Removes all text from the lcd screen.
-	lcd.setCursor() -> Sets the location on the lcd screen to begin showing text.
-	lcd.print() ->Prints the given message to the screen from the cursor location.
### Membrane Switch:
-	keypad.getKey() -> Gets the typed key from keypad as a char.
### Motion Sensor:
-	digitalRead() -> Used to read the motion sensor, either being high or low.
### Piezo Buzzer:
-	tone() -> Starts a tone of the specified frequency on the buzzer.
-	noTone() -> Stops the buzzer from creating a tone.
###Express API:
- transporter.sendMail() -> Sends an email from the Express API to the given email address from the given email address with the subject and text passed.

## Future Improvements
One of the things that can be done to improve the system is to use an external power source. This would allow the system to work separately from a computer, allowing it to be placed anywhere. Another improvement that can be made is to use a more complex input device instead of the membrane switch. This would allow for more complex security codes as well as allowing for new functionality that is not possible with the lack of buttons. For example, it could allow for the ability to input a Wi-Fi name and password through the device instead of it being hardcoded like it is now. It could also allow for the email address to be inputted with the device. Expanding on inputting of otherwise hardcodes values, the system can be improved by allowing the grace period and timeout period to be changed through the system. Finally, at the same time the email is sent, a database can be updated that stores each time the system went off. These are the main improvements I would hope to be able to add given more time. 
