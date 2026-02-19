#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <QString>

class Environment
{
public:
    Environment();
    QString static base_url();
    static QString getKayttosaldo(int tili_id);
    static QString postNosto();
    static QString getTapahtumat(int tili_id);
    static QString getKortti(int kortti_id);
    static QString getAsiakas(int tili_id);
};

#endif // ENVIRONMENT_H
