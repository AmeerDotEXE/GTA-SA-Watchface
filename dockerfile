FROM pebble-setup:latest

WORKDIR /app

COPY . .

CMD [ "bash" ]