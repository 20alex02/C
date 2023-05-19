#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "capture.h"
#include <string.h>

#define UNUSED(x) ((void) x)
#define TEST_FILE "test.pcap"

void print_capture(const struct capture_t *const capture)
{
    for (size_t current_packet = 0; current_packet < packet_count(capture); current_packet++) {
        struct packet_t *packet = get_packet(capture, current_packet);
        printf("\npacket %zu\n", current_packet);
        print_packet_info(packet);
    }
}

void demo1(void)
{
    struct pcap_context context[1];
    if (init_context(context, TEST_FILE) != PCAP_SUCCESS) {
        return;
    }

    struct pcap_header_t *pcap_header = malloc(sizeof(struct pcap_header_t));
    if (pcap_header == NULL) {
        destroy_context(context);
        return;
    }

    if (load_header(context, pcap_header) != PCAP_SUCCESS) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    struct packet_t *packet1 = malloc(sizeof(struct packet_t));

    if (packet1 == NULL) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    if (load_packet(context, packet1) != PCAP_SUCCESS) {
        free(pcap_header);
        destroy_context(context);
        return;
    }

    struct packet_t *packet2 = malloc(sizeof(struct packet_t));

    if (packet2 == NULL) {
        free(packet1);
        free(pcap_header);
        destroy_context(context);
        return;
    }

    if (load_packet(context, packet2) != PCAP_SUCCESS) {
        free(packet1);
        free(pcap_header);
        destroy_context(context);
        return;
    }

    destroy_context(context);

    printf("packet 1:\n");
    print_packet_info(packet1);

    printf("\npacket 2:\n");
    print_packet_info(packet2);

    destroy_packet(packet1);
    destroy_packet(packet2);

    free(packet1);
    free(packet2);
    free(pcap_header);
}

void demo2(void)
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);
    for (size_t current_packet = 0; current_packet < packet_count(capture); current_packet++) {
        struct packet_t *packet = get_packet(capture, current_packet);
        print_packet_info(packet);
    }

    printf("Magic number: 0x%x\n", get_header(capture)->magic_number);
    printf("Total number of bytes transferred in this capture: %zu.\n", data_transfered(capture));
    destroy_capture(capture);
    free(capture);
}

void test1(void)
{
    struct capture_t capture;
    assert(load_capture(&capture, TEST_FILE) == 0);
    assert(capture.header.magic_number == 0xa1b2c3d4);
    assert(get_header(&capture)->magic_number == 0xa1b2c3d4);
    assert(data_transfered(&capture) == 5031);
    assert(packet_count(&capture) == 10);
    struct packet_t packet = *get_packet(&capture, 0);
    assert(packet.packet_header->ts_sec == 1278472579);
    assert(packet.packet_header->ts_usec == 466743);
    assert(packet.packet_header->incl_len == 93);
    assert(packet.packet_header->orig_len == 93);
    assert(packet.eth_header->ether_type == 0x800);
    assert(packet.ip_header->version == 4);
    assert(packet.ip_header->tos == 0);
    assert(packet.ip_header->total_length == 79);
    assert(packet.ip_header->id == 56915);
    assert(packet.ip_header->flags_fo == 64);
    assert(packet.ip_header->ttl == 64);
    assert(packet.ip_header->protocol == 6);
    assert(packet.ip_header->checksum == 18347);
    assert(packet.ip_header->src_addr[0] == 172);
    assert(packet.ip_header->src_addr[1] == 16);
    assert(packet.ip_header->src_addr[2] == 11);
    assert(packet.ip_header->src_addr[3] == 12);
    assert(packet.ip_header->dst_addr[0] == 74);
    assert(packet.ip_header->dst_addr[1] == 125);
    assert(packet.ip_header->dst_addr[2] == 19);
    assert(packet.ip_header->dst_addr[3] == 17);
    destroy_capture(&capture);
}

void test2()
{
    struct pcap_header_t header = {0, 0, 0, 0, 0, 0, 0};
    struct capture_t capture = {header, NULL, NULL, NULL, NULL};
    assert(load_capture(&capture, TEST_FILE) == 0);

    struct capture_t protocol1 = {header, NULL, NULL, NULL, NULL};
    assert(filter_protocol(&capture, &protocol1, 6) == 0);
    assert(packet_count(&protocol1) == 10);
    destroy_capture(&protocol1);

    struct capture_t protocol2 = {header, NULL, NULL, NULL, NULL};
    assert(filter_protocol(&capture, &protocol2, 4) == 0);
    printf("packet count: %lu\n", packet_count(&protocol2));
    assert(packet_count(&protocol2) == 0);
    assert(protocol2.first_node == NULL);
    assert(protocol2.last_node == NULL);
    assert(protocol2.first_flow == NULL);
    assert(protocol2.last_flow == NULL);
    destroy_capture(&protocol2);

    struct capture_t larger1 = {header, NULL, NULL, NULL, NULL};
    assert(filter_larger_than(&capture, &larger1, 70) == 0);
    assert(packet_count(&larger1) == 4);
    destroy_capture(&larger1);

    struct capture_t larger2 = {header, NULL, NULL, NULL, NULL};
    assert(filter_larger_than(&capture, &larger2, 10) == 0);
    assert(packet_count(&larger2) == 10);
    destroy_capture(&larger2);

    struct capture_t larger3 = {header, NULL, NULL, NULL, NULL};
    assert(filter_larger_than(&capture, &larger3, 2000) == 0);
    assert(packet_count(&larger3) == 0);
    assert(larger3.first_node == NULL);
    assert(larger3.last_node == NULL);
    assert(larger3.first_flow == NULL);
    assert(larger3.last_flow == NULL);
    destroy_capture(&larger3);

    struct capture_t from_to1 = {header, NULL, NULL, NULL, NULL};
    uint8_t from[4] = {216, 34, 181, 45};
    uint8_t to[4] = {172, 16, 11, 12};
    assert(filter_from_to(&capture, &from_to1, from, to) == 0);
    assert(packet_count(&from_to1) == 3);
    destroy_capture(&from_to1);

    struct capture_t from_to2 = {header, NULL, NULL, NULL, NULL};
    uint8_t from2[4] = {111, 12, 45, 45};
    uint8_t to2[4] = {172, 16, 11, 12};
    assert(filter_from_to(&capture, &from_to2, from2, to2) == 0);
    assert(packet_count(&from_to2) == 0);
    assert(from_to2.first_node == NULL);
    assert(from_to2.last_node == NULL);
    assert(from_to2.first_flow == NULL);
    assert(from_to2.last_flow == NULL);
    destroy_capture(&from_to2);

    struct capture_t from_mask1 = {header, NULL, NULL, NULL, NULL};
    uint8_t from_mask[4] = {74, 125, 19, 17};
    assert(filter_from_mask(&capture, &from_mask1, from_mask, 8) == 0);
    printf("packet count: %lu \n", packet_count(&from_mask1));
    assert(packet_count(&from_mask1) == 2);
    destroy_capture(&from_mask1);

    struct capture_t from_mask2 = {header, NULL, NULL, NULL, NULL};
    uint8_t from_mask_2[4] = {172, 125, 19, 17};
    assert(filter_from_mask(&capture, &from_mask2, from_mask_2, 1) == 0);
    assert(packet_count(&from_mask2) == 8);
    destroy_capture(&from_mask2);

    struct capture_t to_mask1 = {header, NULL, NULL, NULL, NULL};
    uint8_t to_mask[4] = {74, 125, 19, 17};
    assert(filter_to_mask(&capture, &to_mask1, to_mask, 8) == 0);
    assert(packet_count(&to_mask1) == 3);
    destroy_capture(&to_mask1);

    struct capture_t to_mask2 = {header, NULL, NULL, NULL, NULL};
    uint8_t to_mask_2[4] = {172, 125, 19, 17};
    assert(filter_to_mask(&capture, &to_mask2, to_mask_2, 1) == 0);
    assert(packet_count(&to_mask2) == 7);
    destroy_capture(&to_mask2);
    destroy_capture(&capture);
}

void test_filter_protocol(const char *arg)
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    struct capture_t *filter_capture = malloc(sizeof(struct capture_t));
//    init_filter_capture(capture, filter_capture);

     filter_protocol(capture, filter_capture, 4);

    if (!strcmp(arg, "orig")) {
        printf("ORIG\n");
        print_capture(capture);
    }

    if (!strcmp(arg, "copy")) {
        printf("COPY\n");
        print_capture(filter_capture);
    }

    destroy_capture(capture);
    free(capture);
    destroy_capture(filter_capture);
    free(filter_capture);
}

void test_filter_larger_than(const char *arg)
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    struct capture_t *filter_capture = malloc(sizeof(struct capture_t));
//    init_filter_capture(capture, filter_capture);

    filter_larger_than(capture, filter_capture, 100);

    if (!strcmp(arg, "orig")) {
        printf("ORIG\n");
        print_capture(capture);
    }

    if (!strcmp(arg, "copy")) {
        printf("COPY\n");
        print_capture(filter_capture);
    }

    destroy_capture(capture);
    free(capture);
    destroy_capture(filter_capture);
    free(filter_capture);
}

void test_filter_from_to(const char *arg)
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    struct capture_t *filter_capture = malloc(sizeof(struct capture_t));

    filter_from_to(capture, filter_capture, NULL, NULL);

    if (!strcmp(arg, "orig")) {
        printf("ORIG\n");
//        print_capture(capture);
    }

    if (!strcmp(arg, "copy")) {
        printf("COPY\n");
//        print_capture(filter_capture);
    }

    destroy_capture(capture);
    free(capture);
    destroy_capture(filter_capture);
    free(filter_capture);
}

void filter_from_to_basic()
{
    struct capture_t capture[1];
    load_capture(capture, TEST_FILE);

    struct capture_t filtered[1];

    filter_from_to(
            capture,
            filtered,
            (uint8_t[4]){ 74U, 125U, 19U, 17U },
            (uint8_t[4]){ 172U, 16U, 11U, 12U });

    printf("exp: 2U\nreal: %zu\n", packet_count(filtered));

    destroy_capture(capture);
    destroy_capture(filtered);
}

void print_flow_stats_test()
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    print_flow_stats(capture);
    destroy_capture(capture);
    free(capture);
}

void print_longest_flow_test()
{
    struct capture_t *capture = malloc(sizeof(struct capture_t));
    load_capture(capture, TEST_FILE);

    print_longest_flow(capture);
    destroy_capture(capture);
    free(capture);
}

//void flowstats()
//{
//    int retval = student_main(
//    5,
//    (char *[6]){ "./nft", TEST_FILE, "0.0.0.0/0", "0.0.0.0/0", "flowstats", NULL });
//
//    CHECK(retval == 0);
//    CHECK_FILE(
//    stdout,
//    "172.16.11.12 -> 74.125.19.17 : 3\n"
//    "74.125.19.17 -> 172.16.11.12 : 2\n"
//    "216.34.181.45 -> 172.16.11.12 : 3\n"
//    "172.16.11.12 -> 216.34.181.45 : 2\n");
//}


int main(int argc, const char **argv)
{
//    demo1();
//    demo2();
//    test1();
    test2();
//    print_flow_stats_test();
//    print_longest_flow_test();
//    filter_from_to_basic();
//    if (argc == 2){
//        test_filter_larger_than(argv[1]);
//    }
//    if (argc == 2){
//        test_filter_from_to(argv[1]);
//    }
    return 0;
}

