#!/bin/bash

exec valgrind --show-reachable=yes --leak-check=full ./shivafreetdstester "$@"
