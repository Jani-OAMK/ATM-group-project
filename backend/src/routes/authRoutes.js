//PIN-tarkistus tällä
import express from 'express';
import auth from '../models/authModel.js';
import jwt from 'jsonwebtoken';

const router = express.Router();

router.post('/verify-pin', function (request, response) {

    const { kortti_numero, pin } = request.body;

    if (!kortti_numero || !pin) {
        return response.json({
            success: false,
            message: 'Korttinumero tai PIN puuttuu'
        });
    }

    // hae kortti
    auth.getKorttiByNumero(kortti_numero, function (err, korttiResult) {

        if (err) {
            return response.json(err);
        }

        if (korttiResult.length === 0) {
            return response.json({
                success: false,
                message: 'Korttia ei löydy'
            });
        }

        const kortti = korttiResult[0];

        // tarkista kortin tila
        if (kortti.tila !== 'ACTIVE') {
            return response.json({
                success: false,
                message: 'Kortti ei ole aktiivinen'
            });
        }

        // hae PIN-yritystiedot
        auth.getPinYrityys(kortti.kortti_id, function (err, pinResult) {

            if (err) {
                return response.json(err);
            }

            if (pinResult.length === 0) {
                return response.json({
                    success: false,
                    message: 'PIN-yritystietoja ei löydy'
                });
            }

            const pinYritys = pinResult[0];

            // tarkista aikaperusteinen lukitus
            if (pinYritys.lukossa_asti && new Date(pinYritys.lukossa_asti) > new Date()) {
                return response.json({
                    success: false,
                    message: 'Kortti on väliaikaisesti lukittu'
                });
            }

            // vertaa PIN
            auth.verifyPin(pin, kortti.pin_bcrypt, function (err, isMatch) {

                if (err) {
                    return response.json(err);
                }

                // väärä PIN
                if (!isMatch) {

                    auth.incrementPinError(kortti.kortti_id, function () {

                        if (pinYritys.virhelaskuri + 1 >= 3) {

                            auth.lockPinYrityys(kortti.kortti_id, function () {
                                auth.lockCard(kortti.kortti_id, function () {
                                    return response.json({
                                        success: false,
                                        message: 'Kortti lukittu liian monen virheellisen PIN-yrityksen vuoksi'
                                    });
                                });
                            });

                        } else {
                            return response.json({
                                success: false,
                                message: 'Väärä PIN',
                                yrityksiä_jäljellä: 3 - (pinYritys.virhelaskuri + 1)
                            });
                        }
                    });

                } 
                // oikea PIN
                else {
                    auth.resetPinError(kortti.kortti_id, function () {
                        auth.getKorttiRoolit(kortti.kortti_id, function (err, rooliResult) {
                            if (err) {
                                return response.json(err);
                            }

                            // DEBUG: Tulostetaan mitä tuli
                            console.log('kortti_id:', kortti.kortti_id);
                            console.log('rooliResult:', JSON.stringify(rooliResult));

                            // Tarkista että korttiin on liitetty tilejä
                            if (!rooliResult || rooliResult.length === 0) {
                                return response.json({
                                    success: false,
                                    message: 'Kortille ei ole liitetty tilejä'
                                });
                            }

                            const roles = rooliResult.map(r => r.rooli);

                            let cardType;
                            if (roles.includes('DEBIT') && roles.includes('CREDIT')) {
                                cardType = 'COMBO';
                            } else if (roles.includes('DEBIT')) {
                                cardType = 'DEBIT';
                            } else if (roles.includes('CREDIT')) {
                                cardType = 'CREDIT';
                            } else {
                                // Jos roolit eivät ole DEBIT tai CREDIT, palautetaan virhe
                                return response.json({
                                    success: false,
                                    message: 'Kortin rooli ei ole tuettu. Odotetut roolit: DEBIT tai CREDIT'
                                });
                            }

                            const token = generateAccessToken(kortti.kortti_id);

                            response.json({
                                success: true,
                                message: 'PIN oikein',
                                kortti_id: kortti.kortti_id,
                                token: token,
                                cardType: cardType
                            });
                        });
                    });
                }
            });
        });
    });
});

function generateAccessToken(kortti_id) {
    return jwt.sign({ kortti_id }, process.env.MY_TOKEN, { expiresIn: '1800s' });
}


export default router;
