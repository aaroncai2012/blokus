// Copyright 2019 Aaron Cai for EC327 aaroncai@bu.edu
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
// allowed includes
// tuple, utility, vector, map, set, unordered_map,
// unordered_set, algorithm


using std::cin;
using std::cout;
using std::string;

/* Blokus command glossary

Notes:
 - the first tile is numbered 100, and each new
   one is assigned the next available integer

 - the upper left corner is 0,0

 - first index is row, second index is column


COMMANDS

QUIT: exit the game
>quit
Goodbye

RESET: start the game from beginning
>reset
game reset

CREATE: create a new tile

>create 4
>..*.
>.**.
>.*..
>....
created tile 100


SHOW ALL TILES: show a tile inventory (unordered.)

>show tiles
tile inventory
102
*
101
**
**
100
***
**.
...


SHOW A TILE: show a tile by ID number
>show 100
***
**.
**.

PLAY A TILE: place a tile on the board at row and column numbers

>play 101 4 5
played 101

>play 99 0 0
99 not played

ROTATE A TILE: rotate a tile 90 degrees counterclockwise
>rotate 100
rotate 100 complete
**.
*..
*..

FLIP A TILE sideways (left becomes right)
>fliplr 100
fliplr 100 complete
**.
.*.
.*.

FLIP A TILE vertically (up becomes down)
>flipud 100
flipud 100 complete
.*.
.*.
**.

RESIZE: make the board bigger or smaller. When smaller,
        played pieces fall off the board. Show the new board.

>resize 5

- end of command glossary -
*/


typedef int TileID;

class Tile {
 private:
  std::vector<std::tuple<int, int>> _tiles;
  int _size;
  // common interface. required.
 public:
  Tile(int);
  void show() const;  // print out tile in tilebox format
  void rotate();
  void flipud();
  void fliplr();
  void insert(int x, int y);
  bool is_filled(int x, int y);
  void remove(int x, int y);
  bool is_identical(Tile t); // if orientation and blocks are the same
  bool is_duplicate(Tile t); // if blocks are the same in one orientation
  int size();
};

class Blokus {
 private:
  std::vector<std::tuple<TileID, Tile*>> _tileptrs;
  std::vector<std::tuple<int, int>> _board;
  int _boardsize;
  
  void invalid_tile_error_message();
  void duplicate_tile_error_message(TileID);
  void tile_created_message(TileID);
  void tile_placing_error_message(TileID);
  void tile_placed_message(TileID);
  bool is_right_size(std::vector<string>, int);
  bool has_correct_chars(std::vector<string>);
  int num_filled(std::vector<string>);
  bool are_squares_connected(std::vector<string>);
  std::vector<string> upperleft(std::vector<string>);
  int count_trim(std::vector<string>);
  bool is_in_row(std::vector<string>, int);
  bool is_in_col(std::vector<string>, int);
  std::vector<string> shift_up(std::vector<string>);
  std::vector<string> shift_left(std::vector<string>);

  // common interface. required.
  // collection of Tiles

 public:
  Tile* find_tile(TileID);
  void create_piece();

  void reset();
  void show_tiles() const;
  void show_board() const;
  void play_tile(TileID, int, int);
  void set_size(int);
};

Tile::Tile(int size) {
  _size = size;
}

void Tile::show() const {
    
  // convert vector of tuples to "box".
  std::vector<std::vector<char>> out;
  std::vector<char> charvec;
  for (int i = 0; i<_size; i++){
    charvec.push_back('.');
  }
  for (int i = 0; i < _size; ++i) {
    out.push_back(charvec);
  }

  // set the boxes at the coordinates specified in _tiles to true
  for (auto el : _tiles) {
    out.at(std::get<0>(el)).at(std::get<1>(el)) = '*'; 
  }
  // print
  for (auto row : out){
    for (auto el : row) {
      cout << el;
    }
    cout << "\n";
  }
}

void Tile::rotate() {
    for (auto tu : _tiles) {
        std::get<0>(tu) = -(std::get<1>(tu));
        std::get<1>(tu) = std::get<0>(tu);
    }

}
void Tile::flipud() {
//formula = (size-1)-rowindex
    for (auto tu : _tiles) {
        int x = std::get<0>(tu);
        std::get<0>(tu) = (_size - 1) - x;
        }

}

void Tile::fliplr() {
    for (auto tu : _tiles) {
        int x = std::get<1>(tu);
        std::get<1>(tu) = (_size - 1) - x;
    }

}

void Tile::insert(int x, int y) {
  std::tuple<int, int> tu = std::make_tuple(x, y);
  _tiles.push_back(tu);
}

bool Tile::is_filled(int x, int y) {
  for (int i = 0; i < _tiles.size(); ++i) {
    if (_tiles.at(i) == std::make_tuple(x, y))
      return true;
  }
  return false;
}


bool Tile::is_identical(Tile t) {
    for (int i = 0; i < _tiles.size(); ++i) {
        if (!t.is_filled(std::get<0>(_tiles.at(i)), std::get<1>(_tiles.at(i))))
            return false;
    }
    return true;
}

bool Tile::is_duplicate(Tile t) {
    // check if it is identical
    if (is_identical(t))
        return true;
    // check all orientations on this side
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    // check all orientations on other side
    t.flipud();
    if (is_identical(t))
        return true; 
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    if (is_identical(t))
        return true;
    t.rotate();
    t.flipud(); 
    return false;
}

int Tile::size() {
  return _size;
}

void Blokus::invalid_tile_error_message() {
  cout << "invalid tile" << std::endl;
}

void Blokus::duplicate_tile_error_message(TileID id) {
    cout << "duplicate of " << id << " discarded" << std::endl;
}

void Blokus::tile_created_message(TileID id) {
  cout << "created tile " << id << std::endl;
}

void Blokus::tile_placing_error_message(TileID id) {
  cout << id << " not played" << std::endl;
}

void Blokus::tile_placed_message(TileID id) {
  cout << "played " << id << std::endl;
}

bool Blokus::is_right_size(std::vector<string> in, int size) {
  for (int i = 0; i < in.size(); ++i) {
    // checking for size
    if (in.at(i).size() != size) {
      return false;
    }
  }
  return true;
}

bool Blokus::has_correct_chars(std::vector<string> in) {
  for (int i = 0; i < in.size(); ++i) {
    for (int j = 0; j < in.at(i).size(); ++j) {
      if (in.at(i).at(j) != '*' and in.at(i).at(j) != '.')
        return false;
    }
  }
  return true;
}

int Blokus::num_filled(std::vector<string> in) {
  int sum = 0;
  for (int i = 0; i < in.size(); ++i) {
    for (int j = 0; j < in.at(i).size(); ++j) {
      if (in.at(i).at(j) == '*')
        sum++;
    }
  }
  return sum;
}

bool Blokus::are_squares_connected(std::vector<string> in) {
  // check if all squares are connected
  for (int i = 0; i < in.size(); ++i) {
    for (int j = 0; j < in.at(i).size(); ++j) {
      if (in.at(i).at(j) == '*') {
        bool connection = false;
        // left
        if (i > 0 && in.at(i-1).at(j) == '*')
          connection = true;
        // right
        if (i < in.size()-1 && in.at(i+1).at(j) == '*')
          connection = true; 
        // left
        if (j > 0 && in.at(i).at(j-1) == '*')
          connection = true;
        // up
        if (j < in.at(i).size()-1 && in.at(i).at(j+1))
          connection = true;
        if (connection == false) {
          return false;
        }
      }
    }
  }
  return true;
}

std::vector<string> Blokus::upperleft(std::vector<string> in) {
  while (!is_in_row(in, 0)) {
    in = shift_up(in);
  }
  while (!is_in_col(in, 0)) {
    in = shift_left(in);
  }
  return in;
}

int Blokus::count_trim(std::vector<string> in) {
    int row_trim = 0;
    int col_trim = 0;
    // find row_trim
    for (int i = in.size(); i >= 0; --i) {
        bool is_in_row = false;
        for (int j = 0; j < in.at(i).size(); ++j) {
            if (in.at(i).at(j) == '*')
                is_in_row = true;
        }
        if (is_in_row)
            break;
        row_trim++;
    }
    // find col_trim
    for (int i = in.at(0).size(); i >= 0; --i) {
        bool is_in_col = false;
        for (int j = 0; j < in.size(); ++j) {
            if (in.at(j).at(i) == '*')
                is_in_col = false;
        }
        if (is_in_col)
            break;
        col_trim++;
    }
    if (row_trim < col_trim)
        return row_trim;
    else
        return col_trim;
}

bool Blokus::is_in_row(std::vector<string> in, int row) {
  for (int i = 0; i < in.at(0).size(); ++i) {
    if (in.at(row).at(i) == '*')
      return true;
  }
  return false;
}

bool Blokus::is_in_col(std::vector<string> in, int col) {
  for (int i = 0; i < in.size(); ++i) {
    if (in.at(i).at(col) == '*')
      return true;
  }
  return false;
}

std::vector<string> Blokus::shift_up(std::vector<string> in) {
  in.erase(in.begin());
  string empty(in.at(0).size(), '.');
  in.push_back(empty);
  return in;
}

std::vector<string> Blokus::shift_left(std::vector<string> in) {
  for (int i = 0; i < in.size(); ++i) {
    in.at(i).erase(in.at(i).begin());
    in.at(i).push_back('.');
  }
  return in;
}

Tile* Blokus::find_tile(TileID id) {
  for (int i = 0; i < _tileptrs.size(); ++i) {
    if (std::get<0>(_tileptrs.at(i)) == id)
      return std::get<1>(_tileptrs.at(i));
  }
  std::cerr << "Tile " << id << " does not exist" << std::endl;

}

void Blokus::create_piece() {
  // read in the size
  // read in strings
  // make a Tile
  // store it in a collection of Tiles
  
  // read in the size
  int size;
  cin >> size;
  // read in strings
  std::vector<string> in;
  in.reserve(size);
  for (int i = 0; i < size; ++i) {
    cout << ">";
    cin >> in.at(i);
  }
  // error check
  // check that each rwo is the right size
  if (!is_right_size(in, size)) {
    invalid_tile_error_message();
    return;
  }
  // check that all chars are '*' or '.'
  if (!has_correct_chars(in)) {
    invalid_tile_error_message();
    return;
  }
  // check that there is at least one tile
  if (num_filled(in) < 1) {
    invalid_tile_error_message();
    return;
  }
  // check that all squares are connected
  if (num_filled(in) > 1 and !are_squares_connected(in)) {
    invalid_tile_error_message();
    return;
  }
  // format so tile is in upper left hand corner
  in = upperleft(in);
  // find smallest possible size
  int trimSize = count_trim(in); // size to trim
  int trimmedSize = size - trimSize; // size after trimming
  // make into Tile
  Tile t(trimmedSize);
  for (int i = 0; i < trimmedSize; ++i) {
    for (int j = 0; j < trimmedSize; ++j) {
      if (in.at(i).at(j) == '*')
        t.insert(i, j);
    }
  }
  // check for duplicates
  for (auto ti : _tileptrs) {
      if (std::get<1>(ti)->is_duplicate(t)) {
          duplicate_tile_error_message(std::get<0>(ti));
          return;
      }
  }
  TileID nextid = std::get<0>(_tileptrs.at(_tileptrs.size()-1))++;
  tile_created_message(nextid);
  _tileptrs.push_back(std::make_tuple(nextid, &t));
}

void Blokus::reset() {

}

void Blokus::show_tiles() const {
  for (auto til : _tileptrs) {
    std::get<1>(til)->show();
  }
}

void Blokus::show_board() const {
  // turn tuples into a box
  std::vector<std::vector<bool>> box;
  // make it size of _boardsize
  std::vector<bool> emptyrow;
  for (int i = 0; i < _boardsize; ++i) {
    emptyrow.push_back(false);
  }
  for (int i = 0; i < _boardsize; ++i) {
    box.push_back(emptyrow);
  }
  // populate box with coordinates from _board
  for (auto tu : _board) {
    box.at(std::get<0>(tu)).at(std::get<1>(tu)) = true;
  }
  // print box
  for (auto row : box) {
    for (auto el : row) {
      if (el)
        cout << "*" << std::endl;
      else
        cout << "." << std::endl;
    }
    cout << std::endl;
  }
}

void Blokus::play_tile(TileID id, int x, int y) {
  // check if the tile can be placed
  // check if it will be out of bounds
  if ((x + find_tile(id).size()) > _boardsize) {
    tile_placing_error_message(id);
    return;
  }
  if ((y + find_tile(id).size()) > _boardsize) {
    tile_placing_error_message(id);
    return;
  }
}

void Blokus::set_size(int) {

}

// MAIN. Do not change the below.

int main() {
  string command;
  Blokus b;

  while (true) {
    cin >> command;
    if (command == "quit")  {
      break;
    } else if (command == "//") {
      getline(cin, command);
    } else if (command == "board") {
      b.show_board();
    } else if (command == "create") {
      b.create_piece();
    } else if (command == "reset") {
      b.reset();
    } else if (command == "show") {
      string arg;
      cin >> arg;
      if (arg == "tiles") {
        b.show_tiles();
      } else {
        auto g = b.find_tile(std::stoi(arg));
        g->show();
      }
    } else if (command == "resize") {
      int newsize;
      cin >> newsize;
      b.set_size(newsize);
      b.show_board();
    } else if (command == "play") {
      TileID id;
      int row, col;
      cin >> id >> row >> col;
      b.play_tile(id, row, col);
    } else if (command == "rotate") {
      TileID id;
      cin >> id;
      auto g = b.find_tile(id);
      g->rotate();
      cout << "rotated " << id << "\n";
      g->show();
    } else if (command == "fliplr") {
      TileID id;
      cin >> id;
      auto g = b.find_tile(id);
      g->fliplr();
      cout << "fliplr " << id << "\n";
      g->show();
    } else if (command == "flipud") {
      TileID id;
      cin >> id;
      auto g = b.find_tile(id);
      g->flipud();
      cout << "flipud " << id << "\n";
      g->show();
    } else {
      cout << "command not understood.\n";
    }
  }
  cout << "Goodbye\n";
  return 0;
}