FROM eclipse-mosquitto:2.0.15

# Set password
# - username: test_user
# - password: test

RUN mkdir -p "/etc/mosquitto"
RUN echo "test_user:test" \
    >> /etc/mosquitto/password_file

RUN echo $'\
    persistence false \n\
    connection_messages true \n\
    log_type all \n\
    listener 1883  \n\
    password_file /etc/mosquitto/password_file \n' \
    >> /mosquitto/config/mosquitto.conf

COPY override_entrypoint.sh /override_entrypoint.sh

RUN ["chmod", "+x", "/override_entrypoint.sh"]

ENTRYPOINT ["/override_entrypoint.sh"]
CMD ["/usr/sbin/mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
