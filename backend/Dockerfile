#backend/Dockerfile
FROM node:20-alpine

#envi ja työtila
ENV NODE_ENV=production
WORKDIR /app

#asentaa vaan tuotantoriippuvuudet, hyödynnä cachea
COPY package.json package-lock.json* ./
RUN npm ci --only=production

#sovelluskoodi
COPY src ./src

#sja ei-root -käyttäjänä - kaikkee ei tarvi roottina tehä!
USER node

#srv conffeja
ENV PORT=3000
EXPOSE 3000

CMD ["node", "src/server.js"]
