FROM python:3.12-slim

ARG DEV_UID=1000
ARG DEV_GID=1000
ARG DEV_USERNAME=developer

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

COPY src/dryer-sensor/python/requirements-dev.txt /tmp/requirements-dev.txt
RUN python -m pip install --no-cache-dir -r /tmp/requirements-dev.txt

RUN apt-get update \
    && apt-get install --no-install-recommends -y sudo \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --gid "${DEV_GID}" "${DEV_USERNAME}" \
    && useradd --uid "${DEV_UID}" --gid "${DEV_GID}" \
        --create-home --shell /bin/bash "${DEV_USERNAME}" \
    && printf '%s ALL=(ALL) NOPASSWD:ALL\n' "${DEV_USERNAME}" \
        > "/etc/sudoers.d/${DEV_USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${DEV_USERNAME}"


WORKDIR /workspace
USER ${DEV_USERNAME}

# Keep the container alive for interactive development with `docker compose exec`.
CMD ["sleep", "infinity"]
