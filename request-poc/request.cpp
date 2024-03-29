/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "request.h"
#include "fake_cpp11.h"
#include "options.h"

#include <proton/connection.hpp>
#include <proton/container.hpp>
#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>

#include <thread>         
#include <chrono>        

#include <iostream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <time.h> 

int main(int argc, char** argv) {
	//std::string url("10.33.0.26:61616");
	std::string url("localhost:61616/queue.req-resp");
	std::string address("queue.req-resp");
	std::string user("admin");
	std::string password("admin");

	int totalMessages = 7395;

	try {

		while (true) {
			std::vector<std::string> requests;

			for (int i = 0; i < totalMessages; i++) {
				auto start = std::chrono::system_clock::now();
				auto end = std::chrono::system_clock::now();

				std::chrono::duration<double> elapsed_seconds = end - start;
				std::time_t end_time = std::chrono::system_clock::to_time_t(end);

				std::string timeFormat = std::to_string(end_time);
				timeFormat.insert(10, 235, '0');

				requests.push_back(timeFormat);
			}

			request req(url, address, user, password, requests);
			proton::container(req).run();

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		return 0;
	}
	catch (const example::bad_option & e) {
		std::cout << std::endl << e.what() << std::endl;
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}

	return 1;
}

