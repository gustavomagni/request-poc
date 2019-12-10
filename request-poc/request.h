#include <proton/connection.hpp>
#include <proton/container.hpp>
#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>

#include <iostream>
#include <vector>

using proton::receiver_options;
using proton::source_options;
using proton::sender_options;

#pragma once
class request : public proton::messaging_handler {
private:
	std::string url;
	std::vector<std::string> requests;
	proton::sender sender;
	proton::receiver receiver;

public:
	request(const std::string& u, const std::vector<std::string>& r) : url(u), requests(r) {}

	void on_container_start(proton::container& c) override {
		sender = c.open_sender(url);

		// Create a receiver requesting a dynamically created queue for the message source
		receiver_options opts = receiver_options().source(source_options().dynamic(true));
		receiver = sender.connection().open_receiver("", opts);
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

