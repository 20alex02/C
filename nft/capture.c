#include <stdlib.h>
#include <string.h>
#include "capture.h"

enum filter_type
{
    PROTOCOL,
    LARGER_THAN,
    FROM_TO,
    FROM_MASK,
    TO_MASK
};

uint32_t get_ip(const uint8_t ip[4])
{
    return (ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3];
}

void destroy_flow(struct flow_t *flow)
{
    struct flow_t *next_flow;
    while (flow) {
        next_flow = flow->next;
        free(flow);
        flow = next_flow;
    }
}

struct flow_t *create_flow(struct packet_t packet)
{
    struct flow_t *flow = malloc(sizeof(struct flow_t));
    if (!flow) {
        return NULL;
    }
    memcpy(flow->src, packet.ip_header->src_addr, sizeof(flow->src));
    memcpy(flow->dst, packet.ip_header->dst_addr, sizeof(flow->dst));
    flow->packet_count = 1;
    flow->next = NULL;
    flow->start_sec = packet.packet_header->ts_sec;
    flow->start_usec = packet.packet_header->ts_usec;
    flow->end_sec = packet.packet_header->ts_sec;
    flow->end_usec = packet.packet_header->ts_usec;
    return flow;
}

bool load_flow(struct capture_t *capture, struct packet_t packet)
{
    struct flow_t *flow = capture->first_flow;
    while (flow) {
        if (get_ip(packet.ip_header->src_addr) == get_ip(flow->src) &&
                get_ip(packet.ip_header->dst_addr) == get_ip(flow->dst)) {
            flow->packet_count++;
            flow->end_sec = packet.packet_header->ts_sec;
            flow->end_usec = packet.packet_header->ts_usec;
            return true;
        }
        flow = flow->next;
    }

    struct flow_t *new_flow = create_flow(packet);
    if (!new_flow) {
        return false;
    }
    if (capture->first_flow == NULL || capture->last_flow == NULL) {
        capture->first_flow = new_flow;
        capture->last_flow = new_flow;
    } else {
        capture->last_flow->next = new_flow;
        capture->last_flow = new_flow;
    }
    return true;
}

bool append_node(struct capture_t *capture, struct node_t *node)
{
    struct node_t *new_node = malloc(sizeof(struct node_t));
    if (!new_node) {
        return false;
    }
    capture->last_node = node;
    node->next = new_node;
    return true;
}

void remove_extra_node(struct capture_t *capture, struct node_t *node)
{
    if (capture->first_node->next == NULL) {
        capture->first_node = NULL;
        capture->last_node = NULL;
    } else {
        capture->last_node->next = NULL;
    }
    free(node);
}

int fill_capture(struct pcap_context *context, struct capture_t *capture)
{
    struct node_t *node = capture->first_node;
    int load;
    while ((load = load_packet(context, &(node->packet))) == PCAP_SUCCESS) {
        if (!load_flow(capture, node->packet)) {
            return PCAP_LOAD_ERROR;
        }
        if (!append_node(capture, node)) {
            return PCAP_LOAD_ERROR;
        }
        node = node->next;
    }
    remove_extra_node(capture, node);
    return load;
}

void init_capture(struct capture_t *capture)
{
    capture->first_flow = NULL;
    capture->last_flow = NULL;
    capture->first_node = NULL;
    capture->last_node = NULL;
}

int load_capture(struct capture_t *capture, const char *filename)
{
    init_capture(capture);
    struct pcap_context context[1];
    if (init_context(context, filename) != PCAP_SUCCESS) {
        return -1;
    }
    if (load_header(context, &(capture->header)) != PCAP_SUCCESS) {
        destroy_context(context);
        return -1;
    }

    struct node_t *node = malloc(sizeof(struct node_t));
    if (!node) {
        destroy_context(context);
        return -1;
    }
    node->next = NULL;
    capture->first_node = node;
    capture->last_node = node;
    if (fill_capture(context, capture) == PCAP_LOAD_ERROR) {
        destroy_capture(capture);
        destroy_context(context);
        return -1;
    }
    destroy_context(context);
    return 0;
}

void destroy_nodes(struct node_t *node)
{
    struct node_t *next_node;
    while (node) {
        destroy_packet(&(node->packet));
        next_node = node->next;
        free(node);
        node = next_node;
    }
}

void destroy_capture(struct capture_t *capture)
{
    destroy_nodes(capture->first_node);
    destroy_flow(capture->first_flow);
    capture->first_flow = NULL;
    capture->last_flow = NULL;
    capture->first_node = NULL;
    capture->last_node = NULL;
}

const struct pcap_header_t *get_header(const struct capture_t *const capture)
{
    return &(capture->header);
}

struct packet_t *get_packet(
        const struct capture_t *const capture,
        size_t index)
{
    struct node_t *node = capture->first_node;
    while (index != 0 && node) {
        node = node->next;
        index--;
    }
    if (!node) {
        return NULL;
    }
    return &(node->packet);
}

size_t packet_count(const struct capture_t *const capture)
{
    size_t count = 0;
    struct node_t *node = capture->first_node;
    while (node) {
        node = node->next;
        count++;
    }
    return count;
}

size_t data_transfered(const struct capture_t *const capture)
{
    size_t sum = 0;
    struct node_t *node = capture->first_node;
    while (node) {
        sum += node->packet.packet_header->orig_len;
        node = node->next;
    }
    return sum;
}

bool filter_condition(struct packet_t packet, const uint32_t test_data[2], enum filter_type fltr)
{
    switch (fltr) {
    case PROTOCOL:
        return packet.ip_header->protocol == test_data[0];
    case LARGER_THAN:
        return packet.packet_header->orig_len >= test_data[0];
    case FROM_TO:
        return (get_ip(packet.ip_header->src_addr) == test_data[0]) &&
                (get_ip(packet.ip_header->dst_addr) == test_data[1]);
    case FROM_MASK:
        // got 0 mask len, cant shift 32bit num by 32
        if (test_data[1] == 32) {
            return true;
        }
        return (get_ip(packet.ip_header->src_addr) >> test_data[1]) == (test_data[0] >> test_data[1]);
    case TO_MASK:
        // got 0 mask len, cant shift 32bit num by 32
        if (test_data[1] == 32) {
            return true;
        }
        return (get_ip(packet.ip_header->dst_addr) >> test_data[1]) == (test_data[0] >> test_data[1]);
    default:
        return false;
    }
}

int filter(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t test_data[2],
        enum filter_type fltr)
{
    init_capture(filtered);
    filtered->header = original->header;
    struct node_t *new_node = malloc(sizeof(struct node_t));
    if (!new_node) {
        return -1;
    }
    new_node->next = NULL;
    filtered->first_node = new_node;
    filtered->last_node = new_node;

    struct node_t *node = original->first_node;
    while (node) {
        if (filter_condition(node->packet, test_data, fltr)) {
            if (copy_packet(&(node->packet), &(new_node->packet)) != PCAP_SUCCESS) {
                destroy_capture(filtered);
                return -1;
            }
            if (!append_node(filtered, new_node)) {
                destroy_capture(filtered);
                return -1;
            }
            if (!load_flow(filtered, new_node->packet)) {
                destroy_capture(filtered);
                return -1;
            }
            new_node = new_node->next;
        }
        node = node->next;
    }
    remove_extra_node(filtered, new_node);
    return 0;
}

int filter_protocol(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t protocol)
{
    uint32_t test_data[2] = { protocol };
    return filter(original, filtered, test_data, PROTOCOL);
}

int filter_larger_than(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint32_t size)
{
    uint32_t test_data[2] = { size };
    return filter(original, filtered, test_data, LARGER_THAN);
}

int filter_from_to(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t source_ip[4],
        uint8_t destination_ip[4])
{
    if (!source_ip || !destination_ip) {
        init_capture(filtered);
        return -1;
    }
    uint32_t test_data[2] = { get_ip(source_ip), get_ip(destination_ip) };
    return filter(original, filtered, test_data, FROM_TO);
}

int filter_from_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    if (!network_prefix) {
        init_capture(filtered);
        return -1;
    }
    uint32_t test_data[2] = { get_ip(network_prefix), 32 - mask_length };
    return filter(original, filtered, test_data, FROM_MASK);
}

int filter_to_mask(
        const struct capture_t *const original,
        struct capture_t *filtered,
        uint8_t network_prefix[4],
        uint8_t mask_length)
{
    if (!network_prefix) {
        init_capture(filtered);
        return -1;
    }
    uint32_t test_data[2] = { get_ip(network_prefix), 32 - mask_length };
    return filter(original, filtered, test_data, TO_MASK);
}

int print_flow_stats(const struct capture_t *const capture)
{
    if (!capture) {
        fprintf(stderr, "capture null pointer");
        return -1;
    }
    struct flow_t *flow = capture->first_flow;
    while (flow) {
        uint8_t *src = flow->src;
        uint8_t *dst = flow->dst;
        printf("%hu.%hu.%hu.%hu -> %hu.%hu.%hu.%hu : %zu\n",
                src[0],
                src[1],
                src[2],
                src[3],
                dst[0],
                dst[1],
                dst[2],
                dst[3],
                flow->packet_count);
        flow = flow->next;
    }
    return 0;
}

struct flow_t *find_longest_flow(struct flow_t *flow)
{
    struct flow_t *longest_flow = NULL;
    uint32_t longest_sec = 0;
    uint32_t longest_usec = 0;
    uint32_t sec;
    uint32_t usec;

    while (flow) {
        sec = flow->end_sec - flow->start_sec;
        usec = flow->end_usec - flow->start_usec;
        if (sec > longest_sec || (sec == longest_sec && usec > longest_usec)) {
            longest_flow = flow;
            longest_sec = sec;
            longest_usec = usec;
        }
        flow = flow->next;
    }
    return longest_flow;
}

int print_longest_flow(const struct capture_t *const capture)
{
    if (!capture) {
        fprintf(stderr, "capture null pointer");
        return -1;
    }
    struct flow_t *longest_flow = find_longest_flow(capture->first_flow);
    if (!longest_flow) {
        fprintf(stderr, "no flow found");
        return -1;
    }

    uint8_t *src = longest_flow->src;
    uint8_t *dst = longest_flow->dst;
    printf("%hu.%hu.%hu.%hu -> %hu.%hu.%hu.%hu : %u:%u - %u:%u\n",
            src[0],
            src[1],
            src[2],
            src[3],
            dst[0],
            dst[1],
            dst[2],
            dst[3],
            longest_flow->start_sec,
            longest_flow->start_usec,
            longest_flow->end_sec,
            longest_flow->end_usec);
    return 0;
}
