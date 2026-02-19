#include "environment.h"

Environment::Environment() {}

QString Environment::base_url()
{
    return "https://ankkalinnanpankki.rocks/api/";
}

QString Environment::getKayttosaldo(int tili_id)
{
    return base_url() + "transaktio/kayttosaldo/" + QString::number(tili_id);
}

QString Environment::postNosto()
{
    return base_url() + "transaktio/nosta/";
}

QString Environment::getTapahtumat(int tili_id)
{
    return base_url() + "transaktio/tapahtumat/" + QString::number(tili_id);
}

QString Environment::getKortti(int kortti_id)
{
    return base_url() + "kortti/" + QString::number(kortti_id);
}

QString Environment::getAsiakas(int asiakas_id)
{
    return base_url() + "asiakas/" + QString::number(asiakas_id);
}
