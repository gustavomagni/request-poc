#include <proton/connection.hpp>
#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>
#include <proton/sender_options.hpp>
#include <proton/target_options.hpp>
#include <proton/tracker.hpp>

#include <iostream>
#include <vector>

#include <iostream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <time.h> 
#include <chrono>

using proton::receiver_options;
using proton::source_options;
using proton::sender_options;
using proton::delivery_mode;
using proton::target_options;

#pragma once
class request : public proton::messaging_handler {
private:
	std::string url;
	std::string user;
	std::string password;
	std::string address;

	std::vector<std::string> requests;

	proton::sender sender;
	proton::receiver receiver;
	proton::connection conn_;

	int desired_ = 7395;
	int received_ = 0;

public:
	request(const std::string& url, const std::string& address, const std::string& user, const std::string& password, int desired) :
		url(url),
		address(address),
		user(user),
		password(password),
		desired_(desired)
		{}

	void on_container_start(proton::container& c) override {
		proton::connection_options co;

		if (!user.empty()) co.user(user);
		if (!password.empty()) co.password(password);

		co.sasl_enabled(true);
		co.sasl_allow_insecure_mechs(true);
		
		source_options sourceOpts;
		sourceOpts.address(address);
		//sourceOpts.dynamic(false);

		sender_options opts;
		opts.delivery_mode(delivery_mode::AT_MOST_ONCE);

		sender = c.open_sender(url, opts, co);
			
		// Create a receiver requesting a dynamically created queue for the message source
		receiver_options optsReceiver = receiver_options().source(sourceOpts);
		receiver = sender.connection().open_receiver("", optsReceiver);
	}

	void send_request() {
		proton::message req{getMessage()};
		req.reply_to(receiver.source().address());

		std::cout << "Request" << " => " << req.body() << std::endl;

		sender.send(req);
	}

	void on_receiver_open(proton::receiver&) override {
		send_request();
	}

	void on_message(proton::delivery& d, proton::message& response) override {
		//if (requests.empty()) return; // Spurious extra message!

		std::cout << "Response" << " => " << response.body() << std::endl;
		received_++;
		// requests.erase(requests.begin());

		if (received_ == desired_) {
			std::cout << "SEND CLOSED...";

			d.connection().close();
		}
		else {
			send_request();
		}

	}

	std::string getMessage() {
		auto start = std::chrono::system_clock::now();
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);

		std::string timeFormat = std::to_string(end_time);
		timeFormat.insert(10, 235, '0');

		return timeFormat;
	}
};

