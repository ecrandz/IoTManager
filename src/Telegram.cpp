#include "Consts.h"
#ifdef EnableTelegram
#include "BufferExecute.h"
#include "Telegram.h"
CTBot* myBot{nullptr};

char workstr[25];
int64_t chatID;

void telegramInit() {
    if (isEnableTelegramd()) {
        telegramInitBeen = true;
        sCmd.addCommand("telegram", sendTelegramMsg);
        String token = jsonReadStr(configSetupJson, "telegramApi");
        if (!myBot) {
            myBot = new CTBot();
        }
        myBot->setTelegramToken(token);
        myBot->enableUTF8Encoding(true);
        if (myBot->testConnection()) {
            SerialPrint("I", "Telegram", "Connected");
        } else {
            SerialPrint("E", "Telegram", "Not connected");
        }
        SerialPrint("I", F("Telegram"), F("Telegram Init"));
    }
}

void handleTelegram() {
    if (telegramInitBeen) {
        if (isEnableTelegramd()) {
            if (isTelegramInputOn()) {
                TBMessage msg;
                static unsigned long prevMillis;
                unsigned long currentMillis = millis();
                unsigned long difference = currentMillis - prevMillis;
                if (difference >= 10000) {
                    prevMillis = millis();
                    if (myBot->getNewMessage(msg)) {
                        chatID=msg.sender.id;
                        sprintf(workstr, "%lld", chatID);
                        SerialPrint("->", "Telegram", "chat ID: " + String(workstr) + ", msg: " + String(msg.text));
                        if (jsonReadBool(configSetupJson, "autos")) {
                            jsonWriteLong(configSetupJson, "chatId", chatID);
                            saveConfig();
                        }
                        telegramMsgParse(String(msg.text));
                    }
                }
            }
        }
    }
}

void telegramMsgParse(String msg) {
    if (msg.indexOf("set") != -1) {
        msg = deleteBeforeDelimiter(msg, "_");
        msg.replace("_", " ");
        loopCmdAdd(String(msg) + ",");
        chatID=jsonReadLong(configSetupJson, "chatId");
        sprintf(workstr, "%lld", chatID);
        myBot->sendMessage(chatID, "order done");
        SerialPrint("<-", "Telegram", "chat ID: " + String(workstr) + ", msg: " + String(msg));
    } else if (msg.indexOf("get") != -1) {
        msg = deleteBeforeDelimiter(msg, "_");
        myBot->sendMessage(chatID, getValue(msg));  //jsonReadStr(configLiveJson , msg));
        SerialPrint("<-", "Telegram", "chat ID: " + String(workstr) + ", msg: " + String(msg));
    } else if (msg.indexOf("all") != -1) {
        String list = returnListOfParams();
        myBot->sendMessage(chatID, list);
        SerialPrint("<-", "Telegram", "chat ID: " + String(workstr) + "\n" + list);
    } else {
        myBot->sendMessage(chatID, "ID: " + chipId + ", Name: " + jsonReadStr(configSetupJson, F("name")));
        myBot->sendMessage(chatID, F("Wrong order, use /all to get all values, /get_id to get value, or /set_id_value to set value"));
    }
}

void sendTelegramMsg() {
    String sabject = sCmd.next();
    String msg = sCmd.next();
    chatID=jsonReadLong(configSetupJson, "chatId");
    sprintf(workstr, "%lld", chatID);
    if (sabject == "often") {
        msg.replace("#", " ");
        myBot->sendMessage(jsonReadLong(configSetupJson, "chatId"), msg);
        SerialPrint("<-", "Telegram", "chat ID: " + String(workstr) + ", msg: " + msg);
    } else {
        String prevMsg = jsonReadStr(telegramMsgJson, sabject);
        if (prevMsg != msg) {
            jsonWriteStr(telegramMsgJson, sabject, msg);
            msg.replace("#", " ");
            sabject.replace("#", " ");
            myBot->sendMessage(chatID, sabject + " " + msg);
            SerialPrint("<-", "Telegram", "chat ID: " + String(workstr) + ", msg: " + sabject + " " + msg);
        }
    }
}

bool isEnableTelegramd() {
    return jsonReadBool(configSetupJson, "telegonof");
}

bool isTelegramInputOn() {
    return jsonReadBool(configSetupJson, "teleginput");
}

String returnListOfParams() {
    String cmdStr = readFile(DEVICE_CONFIG_FILE, 4096);
    cmdStr += "\r\n";
    cmdStr.replace("\r\n", "\n");
    cmdStr.replace("\r", "\n");
    int count = 0;
    String out;
    while (cmdStr.length()) {
        String buf = selectToMarker(cmdStr, "\n");
        count++;
        if (count > 1) {
            String id = selectFromMarkerToMarker(buf, ";", 2);
            String value = getValue(id);  //jsonReadStr(configLiveJson , id);
            String page = selectFromMarkerToMarker(buf, ";", 4);
            page.replace("#", " ");
            String name = selectFromMarkerToMarker(buf, ";", 5);
            name.replace("#", " ");
            out += page + " " + " " + name + " " + value + "\n";
        }
        cmdStr = deleteBeforeDelimiter(cmdStr, "\n");
    }
    return out;
}
#endif