FROM eclipse-mosquitto:2.0.15

RUN echo $'\
    persistence false \n\
    allow_anonymous true \n\
    connection_messages true \n\
    log_type all \n\
    listener 1883' >> /mosquitto/config/mosquitto.conf
