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
	std::vector<std::string> requests;
	proton::sender sender;
	proton::receiver receiver;

public:
	request(const std::string& url, const std::string& user, const std::string& password, const std::vector<std::string>& r) : url(url), user(user), password(password), requests(r) {}

	void on_container_start(proton::container& c) override {
		proton::connection_options co;

		if (!user.empty()) co.user(user);
		if (!password.empty()) co.password(password);
		
		source_options sourceOpts;
		sourceOpts.address("valorEconomicoFila");
		sourceOpts.dynamic(false);

		//target_options targetOpts;
		//targetOpts.address("queueAnyCast");
		//targetOpts.dynamic(false);

		sender_options opts;
		opts.delivery_mode(delivery_mode::AT_MOST_ONCE);
		opts.source(sourceOpts);
		//opts.name("Queue");
		//opts.target(targetOpts);

		sender = c.open_sender(url, opts, co);
			
		// Create a receiver requesting a dynamically created queue for the message source
		receiver_options optsReceiver = receiver_options().source(sourceOpts);
		receiver = sender.connection().open_receiver("", optsReceiver);
	}

	void send_request() {
		proton::message req;
		req.body(requests.front());
		req.reply_to(receiver.source().address());

		std::cout << req.body() << std::endl;

		sender.send(req);
	}

	void on_receiver_open(proton::receiver&) override {
		send_request();
	}

	void on_message(proton::delivery& d, proton::message& response) override {
		if (requests.empty()) return; // Spurious extra message!

		std::cout << requests.front() << " => " << response.body() << std::endl;
		requests.erase(requests.begin());

		if (!requests.empty()) {
			send_request();
		}
		else {
			d.connection().close();
		}
	}
};

