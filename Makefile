.PHONY: build docker-shell stop clean rebuild configure build-only test ctest run start \
        configure-inside build-inside test-inside help

DOCKER := docker compose
PRESET := docker-dev-debug
BUILD_DIR := build.docker/dev/debug
SCENE ?= assets/scenes/multiple_spheres.json
CONF_TEST ?= OFF

conan: ; $(DOCKER) run --rm cpp-dev bash -c "conan install . -of=$(BUILD_DIR) --build=missing"

configure: conan
	$(DOCKER) run --rm cpp-dev bash -c "cmake --preset $(PRESET) -U BUILD_TESTING -DBUILD_TESTING=$(CONF_TEST)"

build-only:
	$(DOCKER) run --rm cpp-dev bash -c "cmake --build --preset $(PRESET) -j"

build: CONF_TEST=OFF
build: configure build-only

test: CONF_TEST=ON
test: configure build-only ctest

ctest:
	$(DOCKER) run --rm cpp-dev bash -c "ctest --preset $(PRESET) --output-on-failure"

run:
	$(DOCKER) run --rm cpp-dev ./$(BUILD_DIR)/bin/raylabs $(SCENE)

start: build run

docker-shell: ; $(DOCKER) run --rm cpp-dev
stop: ; $(DOCKER) down -v
clean: ; rm -rf build/ build.docker/ .conan2/
rebuild: clean build

# Inside-container variants
configure-inside: ; cmake --preset $(PRESET) -U BUILD_TESTING -DBUILD_TESTING=$(CONF_TEST)
build-inside: ; cmake --build --preset $(PRESET) -j
test-inside: ; ctest --preset $(PRESET) --output-on-failure

help:
	@echo "Targets: build, test, run, start, ctest, docker-shell, stop, clean, rebuild"
