#ifndef BHH_PARSER_HPP 
#define BHH_PARSER_HPP

#include <kernel.h>
#include <paf.h>
using namespace paf;

typedef enum
{
    VPK,
    DATA
} homebrewType;

typedef struct
{
    paf::String id;
    paf::String titleID;
    paf::WString wstrtitle;
    paf::String title;
    paf::String credits;
    paf::String icon0;
    paf::String download_url;
    paf::String options;
    paf::String icon0Local;
    paf::String description;
    paf::String screenshot_url;
    paf::String version;
} homeBrewInfo;

struct node
{
    homeBrewInfo info;
    node *next;
    widget::ImageButton *button;
    paf::graphics::Texture tex;
};

class linked_list
{
public:
    node *head, *tail;
    linked_list();
    void printall();

    int num;

    void clear();
    homeBrewInfo *add_node();
    void remove_node(const char *tag);
    homeBrewInfo *get(const char *id);
    node *getByNum(int n);

};

void parseJson(const char *path);
void parseCSV(const char *path);

#endif