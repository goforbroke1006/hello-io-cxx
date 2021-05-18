.ONESHELL:
build:
	mkdir -p cmake-build-debug
	cd cmake-build-debug
	cmake ..
	make
	cd ..

load-test: load-test/03-ss-select load-test/04-ss-poll load-test/05-ss-epoll

load-test/03-ss-select:
	bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 10    1 30
	bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 1000  1 30
	bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 2500  1 30
	bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 10000 1 30
	bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 25000 1 30

load-test/04-ss-poll:
	bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 10    1 30
	bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 1000  1 30
	bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 2500  1 30
	bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 10000 1 30
	bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 25000 1 30

load-test/05-ss-epoll:
	bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 10    1 30
	bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 1000  1 30
	bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 2500  1 30
	bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 10000 1 30
	bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 25000 1 30

metrics: \
	metrics/03-ss-select \
	metrics/04-ss-poll \
	metrics/05-ss-epoll \
	metrics/to-md

metrics/03-ss-select: metrics/03-ss-select-10  metrics/03-ss-select-1000  metrics/03-ss-select-2500  metrics/03-ss-select-10000 metrics/03-ss-select-25000

metrics/03-ss-select-10:
	bash get-metrics.sh 03-ss-select-10-output.log
metrics/03-ss-select-1000:
	bash get-metrics.sh 03-ss-select-1000-output.log
metrics/03-ss-select-2500:
	bash get-metrics.sh 03-ss-select-2500-output.log
metrics/03-ss-select-10000:
	bash get-metrics.sh 03-ss-select-10000-output.log
metrics/03-ss-select-25000:
	bash get-metrics.sh 03-ss-select-25000-output.log

metrics/04-ss-poll: metrics/04-ss-poll-10  metrics/04-ss-poll-1000  metrics/04-ss-poll-2500  metrics/04-ss-poll-10000 metrics/04-ss-poll-25000

metrics/04-ss-poll-10:
	bash get-metrics.sh 04-ss-poll-10-output.log
metrics/04-ss-poll-1000:
	bash get-metrics.sh 04-ss-poll-1000-output.log
metrics/04-ss-poll-2500:
	bash get-metrics.sh 04-ss-poll-2500-output.log
metrics/04-ss-poll-10000:
	bash get-metrics.sh 04-ss-poll-10000-output.log
metrics/04-ss-poll-25000:
	bash get-metrics.sh 04-ss-poll-25000-output.log

metrics/05-ss-epoll: metrics/05-ss-epoll-10 metrics/05-ss-epoll-1000 metrics/05-ss-epoll-2500 metrics/05-ss-epoll-10000 metrics/05-ss-epoll-25000

metrics/05-ss-epoll-10:
	bash get-metrics.sh 05-ss-epoll-10-output.log
metrics/05-ss-epoll-1000:
	bash get-metrics.sh 05-ss-epoll-1000-output.log
metrics/05-ss-epoll-2500:
	bash get-metrics.sh 05-ss-epoll-2500-output.log
metrics/05-ss-epoll-10000:
	bash get-metrics.sh 05-ss-epoll-10000-output.log
metrics/05-ss-epoll-25000:
	bash get-metrics.sh 05-ss-epoll-25000-output.log

metrics/to-md:
	bash ./results.sh