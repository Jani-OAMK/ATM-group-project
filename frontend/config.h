#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#define DEBUG_MODE 0 // 0 = debug pois päältä, 1 = debug-tulosteet päällä

#if DEBUG_MODE
    #define DBG() qDebug()
#else
    #define DBG() if (false) qDebug()
#endif

#endif // CONFIG_H
