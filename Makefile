.PHONY: build test

build:
	g++ -o sm_binder sm_binder.cpp binder_class.cpp -I. && g++ -o srv_binder srv_binder.cpp binder_class.cpp -I. && g++ -o cli_binder cli_binder.cpp binder_class.cpp -I. && echo "OK"

test:
	sudo ./test_binder.sh
