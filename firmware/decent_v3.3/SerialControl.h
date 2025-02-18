String inputString = "";  // Almacena el comando recibido
bool stringComplete = false;
bool manualOverride = false; // Indica si el usuario ha cambiado algo manualmente

void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read(); 

        if (inChar == '\n') {  // Fin de comando
            stringComplete = true;
            break;
        } else if (isPrintable(inChar)) {  // Evita caracteres invisibles
            inputString += inChar;
        }
    }
}

void processSerialCommand(String command) {
    command.trim();
    int speed;
    bool validCommand = false; 

    if (command.startsWith("GP1,")) {
        speed = command.substring(4).toInt();
        speedGP1 = constrain(speed, 0, 255);
        analogWrite(GAS_PUMP1, speedGP1);
        gasPump1On = (speedGP1 > 0);
        manualOverride = true;
        validCommand = true;
    }
    else if (command.startsWith("GP2,")) {
        speed = command.substring(4).toInt();
        speedGP2 = constrain(speed, 0, 255);
        analogWrite(GAS_PUMP2, speedGP2);
        gasPump2On = (speedGP2 > 0);
        manualOverride = true;
        validCommand = true;
    }
    else if (command.startsWith("LP1,")) {
        speed = command.substring(4).toInt();
        speedLP1 = constrain(speed, 0, 255);
        analogWrite(LIQUID_PUMP1, speedLP1);
        liquidPump1On = (speedLP1 > 0);
        manualOverride = true;
        validCommand = true;
    }
    else if (command.startsWith("LP2,")) {
        speed = command.substring(4).toInt();
        speedLP2 = constrain(speed, 0, 255);
        analogWrite(LIQUID_PUMP2, speedLP2);
        liquidPump2On = (speedLP2 > 0);
        manualOverride = true;
        validCommand = true;
    }
    else if (command.startsWith("LP3,")) {
        speed = command.substring(4).toInt();
        speedLP3 = constrain(speed, 0, 255);
        analogWrite(LIQUID_PUMP3, speedLP3);
        liquidPump3On = (speedLP3 > 0);
        manualOverride = true;
        validCommand = true;
    }

    if (validCommand) {
        Serial.print("Command received: ");
        Serial.println(command);
    }

    inputString = "";
    stringComplete = false;
}
