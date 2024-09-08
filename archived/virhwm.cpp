/* Created by Namir Garib (E5-44), National Institute of Technology, Hachinohe College */ 

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

#define nmax 64
#define p_max 256
#define l_max 64

//GLOBAL VARIABLES
extern short memory[];
extern int gui(int argc, char **argv);


int font = cv::FONT_HERSHEY_DUPLEX;
cv::Mat img(1400, 2400, CV_8UC3, cv::Scalar(255, 255, 255));
cv::Scalar red = cv::Scalar(0,0,255);
cv::Scalar black = cv::Scalar(0,0,0);
cv::Scalar green = cv::Scalar(0,255,0);
cv::Scalar white = cv::Scalar(255,255,255);

class Branch {
public:
    std::vector<cv::Point> points;
    cv::Scalar color = 0;

    void addPoint(int x, int y) {
        points.push_back(cv::Point(x, y));
    }

    void setColor(int signal) {
        switch(signal){
            case 0:
                color = black;
                break;
            case 1:
                color = red;
                break;
            case 2:
                color = green;
                break;
            default:
                color = black;
                break;
        }
    }

    void draw(cv::Mat& img) {
        for (size_t i = 0; i < points.size() - 1; ++i) {
            cv::line(img, points[i], points[i+1], color, 2);
        }
    }
};

struct Result {
    short C;
    bool ovf;
    bool z;
    bool m;
};

Result falu(short mode, short a, short b) {
    Result result;
    result.C = 0;
    result.ovf = false;
    result.z = false;
    result.m = false;

    switch (mode) {
        case 1:
            result.C = a + b;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            if ((a > 0 && b > 0 && result.C < 0) || (a < 0 && b < 0 && result.C > 0)) result.ovf = true;
            break;

        case 2:
            result.C = a - b;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            if ((a > 0 && b < 0 && result.C < 0) || (a < 0 && b > 0 && result.C > 0)) result.ovf = true;
            break;

        case 3:
            result.C = b - a;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            if ((a > 0 && b < 0 && result.C > 0) || (a < 0 && b > 0 && result.C < 0)) result.ovf = true;
            break;

        case 4:
            result.C = a || b;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            break;

        case 5:
            result.C = a && b;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            break;

        case 6:
            result.C = a ^ b;
            if (result.C == 0) result.z = true;
            if (result.C < 0) result.m = true;
            break;

        case 7:
            result.C = (a & 0x8000) != 0;
            result.C = a >> 1;
            result.C |= (a & 0x8000); // Keep the sign bit
            result.m = (result.C & 0x8000) != 0;
            result.z = result.C == 0;
            break;

        case 8:
            result.C = a << 1;
            result.C = (a & 0x0001) != 0;
            result.C = a << 1;
            result.C &= 0xFFFF; // Ensure it stays 16-bit
            result.m = (result.C & 0x8000) != 0;
            result.z = result.C == 0;
            break;

        default:
            printf("Invalid mode\n");
            exit(EXIT_FAILURE);
    }

    return result;
}

short mpy(int m1, int m2)
{
	return m1 * m2;
}

struct MIP {
    short Al, Ao, A1o, A0o, MA1i, MA1l, A1I, MA1o;
    short Byo,MD1i,MD1l,BMD1o,MD2i,MD2l,MA2l,A2I,BMPYo,Lpl,Lpc;
    short r_w, memo, MMD1o, MMD2o, alu, Deo, Beo, ovf, zf, mf, ncld, stp, emit;
};

MIP mip[] = {
        {0,1,1,1,0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,-1},
		{0,1,1,1,0,0,0,1,1,0,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,7},
		{0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,8},
		{0,1,1,0,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0},
		{0,1,1,1,0,0,1,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,1,1,1,1,0,0,0,0,0,0},
		{0,1,1,1,0,0,0,1,1,2,1,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
		{0,1,1,1,0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,63},
		{0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
		{0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,0}
};

void onMouse(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_MOUSEMOVE)
    {
        // Cast userdata to cv::Mat
        cv::Mat* img = static_cast<cv::Mat*>(userdata);

        // Clear the area where the coordinates are displayed
        cv::rectangle(*img, cv::Point(0, 0), cv::Point(300, 50), cv::Scalar(255, 255, 255), -1);

        // Create the text string
        std::stringstream ss;
        ss << "(" << x << ", " << y << ")";
        std::string coords = ss.str();

        // Draw the coordinates on the image
        cv::putText(*img, coords, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

        // Refresh the window
        cv::imshow("img", *img);
    }
}

void drawPreviewMemory(cv::Mat& img, const std::vector<short>& A, const std::vector<short>& B) {
    int startX = 50;
    int startY = img.rows - 200;
    int cellWidth = 80;
    int cellHeight = 40;

    //Clear the area where the memory is displayed
    cv::rectangle(img, cv::Point(startX, startY), cv::Point(startX + 10 * cellWidth, startY + 2 * cellHeight), white, -1);

    // Draw table
    for (int i = 0; i <= 9; ++i) {
        cv::rectangle(img, cv::Point(startX, startY), cv::Point(startX + (i) * cellWidth, startY + 2 * cellHeight), black, 2);
        cv::line(img, cv::Point(startX, startY + cellHeight), cv::Point(startX + i * cellWidth, startY + cellHeight), black, 2);
    }

    // Draw labels
    cv::putText(img, "A", cv::Point(startX + cellWidth / 2, startY + cellHeight / 2 + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, black, 2);
    cv::putText(img, "B", cv::Point(startX + cellWidth / 2, startY + 3 * cellHeight / 2 + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, black, 2);

    // Draw contents of A and B
    for (int i = 0; i < 8; ++i) {
        cv::putText(img, std::to_string(A[i]), cv::Point(startX + (i+1) * cellWidth + cellWidth / 2, startY + cellHeight / 2 + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, black, 2);
        cv::putText(img, std::to_string(B[i]), cv::Point(startX + (i+1) * cellWidth + cellWidth / 2, startY + 3 * cellHeight / 2 + 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, black, 2);
    }
}

void showValue(cv::Mat& img, std::string name, short value, int x, int y, cv::Scalar color = black) {
    // Text alignment
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(name, cv::FONT_HERSHEY_SIMPLEX, 1, 2, &baseline);

     // Clear the area where the coordinates are displayed
    cv::rectangle(img, cv::Point(x, y - textSize.height), cv::Point(x + textSize.width + 50, y + baseline), cv::Scalar(255, 255, 255), -1);

    // Draw label
    cv::putText(img, name, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2);

    // Draw contents
    cv::putText(img, std::to_string(value), cv::Point(x + textSize.width + 10, y), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2);
}

void clearValue(cv::Mat& img, std::string name, int x, int y, cv::Scalar bgColor = white) {
    // Calculate the width of the label text
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(name, cv::FONT_HERSHEY_SIMPLEX, 1, 2, &baseline);

    // Clear the area
    cv::rectangle(img, cv::Point(x, y - textSize.height - 5), cv::Point(x + textSize.width + 90, y + baseline), bgColor, -1);
}

void drawNode(int x, int y, cv::Scalar color = cv::Scalar(0, 0, 0)) {
    cv::Point center = cv::Point(x, y);
    int radius = 4;
    cv::circle(img, center, radius, color, cv::FILLED);
}

class Device{
    public:
        cv::Scalar color = black;
    
    void setColor(int signal) {
        switch(signal){
            case 0:
                color = black;
                break;
            case 1:
                color = red;
                break;
            case 2:
                color = green;
                break;
            default:
                color = black;
                break;
        }
    }
    
    void data_memory(int i, int j) {
    cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+80), color, 2);
        cv::putText(img, "DATA", cv::Point(i+80, j+28), font, 1, color, 2);
        cv::putText(img, "MEMORY", cv::Point(i+50, j+63), font, 1, color, 2);
    }

    void branch_control(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+80), color, 2);
        cv::putText(img, "BRANCH", cv::Point(i+60, j+28), font, 1, color, 2);
        cv::putText(img, "CONTROL", cv::Point(i+50, j+63), font, 1, color, 2);
    }

    void ma1_register(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2);
        cv::putText(img, "MA1-reg", cv::Point(i+60, j+28), font, 1, color, 2);
    }

    void ma2_register(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2);
        cv::putText(img, "MA2-reg", cv::Point(i+60, j+28), font, 1, color, 2);
    }

    void md1_register(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2);
        cv::putText(img, "MD1-reg", cv::Point(i+60, j+28), font, 1, color, 2);
    }

    void md2_register(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2);
        cv::putText(img, "MD2-reg", cv::Point(i+60, j+28), font, 1, color, 2);
    }

    void a_register(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2); // i = 400, j = 440
        cv::putText(img, "A-reg", cv::Point(i+80, j+28), font, 1, color, 2);
    }

    void multiplier(int i, int j) {
        cv::line(img, cv::Point(i,j), cv::Point(i+100, j),color,2);
        cv::line(img, cv::Point(i+100,j), cv::Point(i+120, j+20),color,2);
        cv::line(img, cv::Point(i+120,j+20), cv::Point(i+140, j),color,2);
        cv::line(img, cv::Point(i+140,j), cv::Point(i+240, j),color,2);
        cv::line(img, cv::Point(i+240,j), cv::Point(i+180, j+60),color,2); //
        cv::line(img, cv::Point(i+180,j+60), cv::Point(i+60, j+60),color,2);
        cv::line(img, cv::Point(i+60,j+60), cv::Point(i, j),color,2);
        cv::putText(img, "MPY", cv::Point(i+90, j+52), font, 1, color, 2);
    }

    void alu(int i, int j) {
        cv::line(img, cv::Point(i,j), cv::Point(i+100, j),color,2);
        cv::line(img, cv::Point(i+100,j), cv::Point(i+120, j+20),color,2);
        cv::line(img, cv::Point(i+120,j+20), cv::Point(i+140, j),color,2);
        cv::line(img, cv::Point(i+140,j), cv::Point(i+240, j),color,2);
        cv::line(img, cv::Point(i+240,j), cv::Point(i+180, j+60),color,2); //
        cv::line(img, cv::Point(i+180,j+60), cv::Point(i+60, j+60),color,2);
        cv::line(img, cv::Point(i+60,j+60), cv::Point(i, j),color,2);
        cv::putText(img, "ALU", cv::Point(i+90, j+52), font, 1, color, 2);
    }

    void program_counter(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+80), color, 2);
        cv::putText(img, "PROGRAM", cv::Point(i+50, j+28), font, 1, color, 2);
        cv::putText(img, "COUNTER", cv::Point(i+50, j+63), font, 1, color, 2);
    }

    void program_memory(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+80), color, 2);
        cv::putText(img, "PROGRAM", cv::Point(i+50, j+28), font, 1, color, 2);
        cv::putText(img, "MEMORY", cv::Point(i+60, j+63), font, 1, color, 2);
    }

    void loop_register(int i, int j) {
        cv:rectangle(img, cv::Point(i, j), cv::Point(i+80, j+40), color, 2);
        cv::putText(img, "loop", cv::Point(i+10, j+32), font, 1, color, 2);
    }

    void clock(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+40, j+40), color, 2);
        cv::putText(img, "CLK", cv::Point(i+3, j+28), font, 0.6, color, 2);
    }

    void tri_south(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+60, j+50), color, 2);
        cv::line(img, cv::Point(i+10,j+10), cv::Point(i+30,j+50), color,2);
        cv::line(img, cv::Point(i+30,j+50), cv::Point(i+50,j+10), color,2);
        cv::line(img, cv::Point(i+50,j+10), cv::Point(i+10,j+10), color,2);
        cv::circle(img, cv::Point(i+48,j+30), 6, cv::Scalar(0, 0, 0),2);
        cv::line(img, cv::Point(i+53,j+30), cv::Point(i+70,j+30), color,2);
    }

    void tri_north(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+60, j+50), color, 2);
        cv::line(img, cv::Point(i+30,j+0), cv::Point(i+10,j+40), color,2);
        cv::line(img, cv::Point(i+10,j+40), cv::Point(i+50,j+40), color,2);
        cv::line(img, cv::Point(i+50,j+40), cv::Point(i+30,j+0), color,2);
        cv::circle(img, cv::Point(i+48,j+20), 6, color,2);
        cv::line(img, cv::Point(i+53,j+20), cv::Point(i+70,j+20), color,2);
    }

    void tri_west(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+50, j+60), color, 2);
        cv::line(img, cv::Point(i,j+30), cv::Point(i+50,j+50), color,2);
        cv::line(img, cv::Point(i+50,j+50), cv::Point(i+50,j+10), color,2);
        cv::line(img, cv::Point(i+50,j+10), cv::Point(i,j+30), color,2);
        cv::circle(img, cv::Point(i+20,j+45), 6, cv::Scalar(0, 0, 0),2);
        cv::line(img, cv::Point(i+20,j+49), cv::Point(i+20,j+70), color,2);
    }

    void selector(int i, int j) {
        cv::rectangle(img, cv::Point(i, j), cv::Point(i+240, j+40), color, 2);
        cv::putText(img, "SEL", cv::Point(i+100, j+28), font, 1, color, 2);
    }

    
};


int main(int argc, char **argv) {
    int result = gui(argc, argv);

    // ***** DEFINING OBJECTS *****/
    Device data_memory;
    Device a_register;

    Device ma1_register;
    Device ma1_selector;

    Device md1_register;
    Device md1_selector;

    Device ma2_register;
    
    Device md2_register;
    Device md2_selector;

    Device multiplier;
    Device alu;
    Device branch_control;
    Device program_counter;
    Device clock;
    Device program_memory;
    Device tri_north;
    Device tri_south;
    Device loop_register;
    Device tri_west;


    //*** CREATING ALL COMPONENTS *********************************/
    data_memory.data_memory(1110, 100);         // 240x80
    a_register.a_register(150, 400);           // 240x40
    ma1_selector.selector(510, 320);             // 240x40 //ma1_selector
    ma1_register.ma1_register(510, 400);         // 240x40
    md1_selector.selector(870, 320);             // 240x40 //md1_selector
    md1_register.md1_register(870, 400);         // 240x40
    md2_selector.selector(1350, 320);            // 240x40 //md2_selector
    md2_register.md2_register(1350, 400);        // 240x40
    ma2_register.ma2_register(1710, 400);        // 240x40
    multiplier.multiplier(1110, 660);                 // 240x60
    alu.alu(610, 900);                  // 240x60
    branch_control.branch_control(1110, 1000);     // 240x80
    program_counter.program_counter(1610, 1000);    // 240x80
    clock.clock(1500, 1000);              // 40x40
    program_memory.program_memory(1610, 1130);     // 240x80
    tri_north.tri_north(400, 910);            // left to the alu
    tri_south.tri_south(770, 700);            // above alu
    loop_register.loop_register(1000, 900);            // right to the alu
    tri_west.tri_west(2040,190);             //right upper corner
    tri_west.tri_west(2040,830);             //right lower corner
    tri_south.tri_south(240, 500);            //below a_register
    tri_south.tri_south(630, 500);            //below ma1_register
    tri_south.tri_south(1080, 500);            //below md1_register
    tri_south.tri_south(1320, 500);           //below md2_register
    tri_south.tri_south(1200, 770);           //below mpy
    /*************************************************************/

    //********** DRAWING NODES  **********/
    drawNode(660, 860);     // a_bus
    drawNode(430, 860);     // a_bus
    drawNode(430, 1010);    // below buffer, on alu line
    drawNode(270, 280);     // alu_output
    drawNode(550, 280);     // alu_output
    drawNode(660, 470);     // 
    drawNode(990, 220);     // 
    drawNode(1190, 280);
    drawNode(1270, 280);
    drawNode(1550, 220);
    drawNode(1830, 220);
    drawNode(990, 470);
    drawNode(1470, 470);
    drawNode(800, 860);
    drawNode(1040, 860);
    drawNode(1610, 1020);

    //********** CREATING BRANCHES **********/
    Branch databus;
    databus.addPoint(710, 320);
    databus.addPoint(700, 310);
    databus.addPoint(710, 320);
    databus.addPoint(720, 310);
    databus.addPoint(710, 320);
    databus.addPoint(710, 220);
    databus.addPoint(990, 220);
    databus.addPoint(990, 320);
    databus.addPoint(980, 310);
    databus.addPoint(990, 320);
    databus.addPoint(1000, 310);
    databus.addPoint(990, 320);
    databus.addPoint(990, 220);
    databus.addPoint(1550, 220);
    databus.addPoint(1550, 320);
    databus.addPoint(1540, 310);
    databus.addPoint(1550, 320);
    databus.addPoint(1560, 310);
    databus.addPoint(1550, 320);
    databus.addPoint(1550, 220);
    databus.addPoint(1830, 220);
    databus.addPoint(1830, 400);
    databus.addPoint(1820, 390);
    databus.addPoint(1830, 400);
    databus.addPoint(1840, 390);
    databus.addPoint(1830, 400);
    databus.addPoint(1830, 220);
    databus.addPoint(2040, 220);
    databus.draw(img);       // Draw abus

    Branch a_bus;
    a_bus.addPoint(270, 860);
    a_bus.addPoint(660, 860);
    a_bus.addPoint(660, 900);
    a_bus.addPoint(650, 890);
    a_bus.addPoint(660, 900);
    a_bus.addPoint(670, 890);
    a_bus.addPoint(660, 900);
    a_bus.draw(img);       // Draw abus

    Branch b_bus;
    b_bus.addPoint(800, 900);
    b_bus.addPoint(800, 860);
    b_bus.addPoint(1040, 860);
    b_bus.addPoint(1040, 900);
    b_bus.addPoint(1030, 890);
    b_bus.addPoint(1040, 900);
    b_bus.addPoint(1050, 890);
    b_bus.addPoint(1040, 900);
    b_bus.addPoint(1040, 860);
    b_bus.addPoint(2040, 860);
    b_bus.draw(img);       // Draw abus

    Branch sel_to_ma1;
    sel_to_ma1.addPoint(630, 360);
    sel_to_ma1.addPoint(630, 400);
    sel_to_ma1.addPoint(620, 390);
    sel_to_ma1.addPoint(630, 400);
    sel_to_ma1.addPoint(640, 390);
    sel_to_ma1.addPoint(630, 400);
    sel_to_ma1.draw(img);
    

    Branch sel_to_md1;
    sel_to_md1.addPoint(990, 360);
    sel_to_md1.addPoint(990, 400);
    sel_to_md1.addPoint(980, 390);
    sel_to_md1.addPoint(990, 400);
    sel_to_md1.addPoint(1000, 390);
    sel_to_md1.addPoint(990, 400);
    sel_to_md1.draw(img);

    Branch sel_to_md2;
    sel_to_md2.addPoint(1470, 360);
    sel_to_md2.addPoint(1470, 400);
    sel_to_md2.addPoint(1460, 390);
    sel_to_md2.addPoint(1470, 400);
    sel_to_md2.addPoint(1480, 390);
    sel_to_md2.addPoint(1470, 400);
    sel_to_md2.draw(img);

    Branch ma1_to_data_memory;
    ma1_to_data_memory.addPoint(660, 440);
    ma1_to_data_memory.addPoint(660, 470);
    ma1_to_data_memory.addPoint(460, 470);
    ma1_to_data_memory.addPoint(460, 140);
    ma1_to_data_memory.addPoint(1110, 140);
    ma1_to_data_memory.addPoint(1100, 130);
    ma1_to_data_memory.addPoint(1110, 140);
    ma1_to_data_memory.addPoint(1100, 150);
    ma1_to_data_memory.draw(img);

    Branch alu_output;
    alu_output.addPoint(730, 960);
    alu_output.addPoint(730, 1010);
    alu_output.addPoint(430, 1010);
    alu_output.addPoint(430, 960);
    alu_output.addPoint(430, 1010);
    alu_output.addPoint(100, 1010);
    alu_output.addPoint(100, 280);
    alu_output.addPoint(270, 280);
    alu_output.addPoint(270, 400);
    alu_output.addPoint(260, 390);
    alu_output.addPoint(270, 400);
    alu_output.addPoint(280, 390);
    alu_output.addPoint(270, 400);
    alu_output.addPoint(270, 280);
    alu_output.addPoint(550, 280);
    alu_output.addPoint(550, 320);
    alu_output.addPoint(540, 310);
    alu_output.addPoint(550, 320);
    alu_output.addPoint(560, 310);
    alu_output.addPoint(550, 320);
    alu_output.addPoint(550, 280);
    alu_output.addPoint(910, 280);
    alu_output.addPoint(910, 320);
    alu_output.addPoint(900, 310);
    alu_output.addPoint(910, 320);
    alu_output.addPoint(920, 310);
    alu_output.addPoint(910, 320);
    alu_output.draw(img);
    
    Branch alu_to_branch_control;
    alu_to_branch_control.addPoint(820, 930);
    alu_to_branch_control.addPoint(870, 930);
    alu_to_branch_control.addPoint(870, 1040);
    alu_to_branch_control.addPoint(1110, 1040);
    alu_to_branch_control.addPoint(1100, 1030);
    alu_to_branch_control.addPoint(1110, 1040);
    alu_to_branch_control.addPoint(1100, 1050);
    alu_to_branch_control.addPoint(1110, 1040);
    alu_to_branch_control.draw(img);

    Branch clk_to_program_counter;
    clk_to_program_counter.addPoint(1540, 1020);
    clk_to_program_counter.addPoint(1610, 1020);
    clk_to_program_counter.draw(img);

    Branch pc_to_pm;
    pc_to_pm.addPoint(1730, 1080);
    pc_to_pm.addPoint(1730, 1130);
    pc_to_pm.addPoint(1720, 1120);
    pc_to_pm.addPoint(1730, 1130);
    pc_to_pm.addPoint(1740, 1120);
    pc_to_pm.addPoint(1730, 1130);
    pc_to_pm.draw(img);

    Branch branch_control_to_pc;
    branch_control_to_pc.addPoint(1350, 1060);
    branch_control_to_pc.addPoint(1610, 1060);
    branch_control_to_pc.draw(img);

    Branch alu_buffer_to_a_bus;
    alu_buffer_to_a_bus.addPoint(430, 860);
    alu_buffer_to_a_bus.addPoint(430, 910);
    alu_buffer_to_a_bus.draw(img);

    Branch a_reg_to_buffer;
    a_reg_to_buffer.addPoint(270, 440);
    a_reg_to_buffer.addPoint(270, 500);
    a_reg_to_buffer.addPoint(260, 490);
    a_reg_to_buffer.addPoint(270, 500);
    a_reg_to_buffer.addPoint(280, 490);
    a_reg_to_buffer.addPoint(270, 500);
    a_reg_to_buffer.draw(img);

    Branch a_buffer_to_a_bus;
    a_buffer_to_a_bus.addPoint(270, 550);
    a_buffer_to_a_bus.addPoint(270, 860);
    a_buffer_to_a_bus.draw(img);

    Branch ma1_reg_to_ma1_buffer;
    ma1_reg_to_ma1_buffer.addPoint(660, 440);
    ma1_reg_to_ma1_buffer.addPoint(660, 500);
    ma1_reg_to_ma1_buffer.addPoint(650, 490);
    ma1_reg_to_ma1_buffer.addPoint(660, 500);
    ma1_reg_to_ma1_buffer.addPoint(670, 490);
    ma1_reg_to_ma1_buffer.addPoint(660, 500);
    ma1_reg_to_ma1_buffer.draw(img);

    Branch ma1_buffer_to_alu;
    ma1_buffer_to_alu.addPoint(660, 550);
    ma1_buffer_to_alu.addPoint(660, 860);
    ma1_buffer_to_alu.draw(img);

    Branch data_mem_to_md1_sel; //branch going from data memory to md1 selector
    data_mem_to_md1_sel.addPoint(1180, 190);
    data_mem_to_md1_sel.addPoint(1190, 180);
    data_mem_to_md1_sel.addPoint(1200, 190);
    data_mem_to_md1_sel.addPoint(1190, 180);
    data_mem_to_md1_sel.addPoint(1190, 280);
    data_mem_to_md1_sel.addPoint(1070, 280);
    data_mem_to_md1_sel.addPoint(1070, 320);
    data_mem_to_md1_sel.addPoint(1060, 310);
    data_mem_to_md1_sel.addPoint(1070, 320);
    data_mem_to_md1_sel.addPoint(1080, 310);
    data_mem_to_md1_sel.addPoint(1070, 320);
    data_mem_to_md1_sel.draw(img);

    Branch data_mem_to_md2_sel; //branch going from data memory to md2 selector
    data_mem_to_md2_sel.addPoint(1260, 190);
    data_mem_to_md2_sel.addPoint(1270, 180);
    data_mem_to_md2_sel.addPoint(1280, 190);
    data_mem_to_md2_sel.addPoint(1270, 180);
    data_mem_to_md2_sel.addPoint(1270, 280);
    data_mem_to_md2_sel.addPoint(1390, 280);
    data_mem_to_md2_sel.addPoint(1390, 320);
    data_mem_to_md2_sel.addPoint(1380, 310);
    data_mem_to_md2_sel.addPoint(1390, 320);
    data_mem_to_md2_sel.addPoint(1400, 310);
    data_mem_to_md2_sel.addPoint(1390, 320);
    data_mem_to_md2_sel.draw(img);

    Branch md1_buff_to_data_mem; //branch going from md1 buffer to data memory
    md1_buff_to_data_mem.addPoint(1110, 550);
    md1_buff_to_data_mem.addPoint(1110, 590);
    md1_buff_to_data_mem.addPoint(1190, 590);
    md1_buff_to_data_mem.addPoint(1190, 180);
    md1_buff_to_data_mem.draw(img);

    Branch md2_buff_to_data_mem; //branch going from md2 buffer to data memory
    md2_buff_to_data_mem.addPoint(1350, 550);
    md2_buff_to_data_mem.addPoint(1350, 590);
    md2_buff_to_data_mem.addPoint(1270, 590);
    md2_buff_to_data_mem.addPoint(1270, 180);
    md2_buff_to_data_mem.draw(img);

    Branch md1_to_mpy; //branch going from md1 register to mpy
    md1_to_mpy.addPoint(990, 440);
    md1_to_mpy.addPoint(990, 620);
    md1_to_mpy.addPoint(1160, 620);
    md1_to_mpy.addPoint(1160, 660);
    md1_to_mpy.addPoint(1150, 650);
    md1_to_mpy.addPoint(1160, 660);
    md1_to_mpy.addPoint(1170, 650);
    md1_to_mpy.addPoint(1160, 660);
    md1_to_mpy.draw(img);

    Branch md2_to_mpy; //branch going from md2 register to mpy
    md2_to_mpy.addPoint(1470, 440);
    md2_to_mpy.addPoint(1470, 620);
    md2_to_mpy.addPoint(1290, 620);
    md2_to_mpy.addPoint(1290, 660);
    md2_to_mpy.addPoint(1280, 650);
    md2_to_mpy.addPoint(1290, 660);
    md2_to_mpy.addPoint(1300, 650);
    md2_to_mpy.addPoint(1290, 660);
    md2_to_mpy.draw(img);

    Branch md1_to_buffer; //branch going from md1 register to md1 buffer
    md1_to_buffer.addPoint(990, 440);
    md1_to_buffer.addPoint(990, 470);
    md1_to_buffer.addPoint(1110, 470);
    md1_to_buffer.addPoint(1110, 500);
    md1_to_buffer.addPoint(1100, 490);
    md1_to_buffer.addPoint(1110, 500);
    md1_to_buffer.addPoint(1120, 490);
    md1_to_buffer.addPoint(1110, 500);
    md1_to_buffer.draw(img);

    Branch md1_to_alu_buffer; //branch going from md1 register to alu buffer
    md1_to_alu_buffer.addPoint(990, 440);
    md1_to_alu_buffer.addPoint(990, 470);
    md1_to_alu_buffer.addPoint(800, 470);
    md1_to_alu_buffer.addPoint(800, 700);
    md1_to_alu_buffer.addPoint(790, 690);
    md1_to_alu_buffer.addPoint(800, 700);
    md1_to_alu_buffer.addPoint(810, 690);
    md1_to_alu_buffer.addPoint(800, 700);
    md1_to_alu_buffer.draw(img);

    Branch alu_buff_to_alu; //branch going from alu buffer (the one to the left of md1 reg output) to alu
    alu_buff_to_alu.addPoint(800, 750);
    alu_buff_to_alu.addPoint(800, 900);
    alu_buff_to_alu.addPoint(790, 890);
    alu_buff_to_alu.addPoint(800, 900);
    alu_buff_to_alu.addPoint(810, 890);
    alu_buff_to_alu.addPoint(800, 900);
    alu_buff_to_alu.draw(img);

    Branch md2_to_buff; //branch going from md2 register to md2 buffer
    md2_to_buff.addPoint(1470, 440);
    md2_to_buff.addPoint(1470, 470);
    md2_to_buff.addPoint(1350, 470);
    md2_to_buff.addPoint(1350, 500);
    md2_to_buff.addPoint(1340, 490);
    md2_to_buff.addPoint(1350, 500);
    md2_to_buff.addPoint(1360, 490);
    md2_to_buff.addPoint(1350, 500);
    md2_to_buff.draw(img);
    

    Branch ma2_to_data_mem; //branch going from ma2 register to data memory
    ma2_to_data_mem.addPoint(1830, 440);
    ma2_to_data_mem.addPoint(1830, 480);
    ma2_to_data_mem.addPoint(2000, 480);
    ma2_to_data_mem.addPoint(2000, 140);
    ma2_to_data_mem.addPoint(1350, 140);
    ma2_to_data_mem.addPoint(1360, 130);
    ma2_to_data_mem.addPoint(1350, 140);
    ma2_to_data_mem.addPoint(1360, 150);
    ma2_to_data_mem.draw(img);

    Branch mpy_to_buff; //branch going from mpy to buffer
    mpy_to_buff.addPoint(1230, 720);
    mpy_to_buff.addPoint(1230, 770);
    mpy_to_buff.addPoint(1220, 760);
    mpy_to_buff.addPoint(1230, 770);
    mpy_to_buff.addPoint(1240, 760);
    mpy_to_buff.addPoint(1230, 770);
    mpy_to_buff.draw(img);

    Branch mpy_buff_to_bbus; //branch going from mpy buffer to b bus
    mpy_buff_to_bbus.addPoint(1230, 820);
    mpy_buff_to_bbus.addPoint(1230, 860);
    mpy_buff_to_bbus.addPoint(1220, 850);
    mpy_buff_to_bbus.addPoint(1230, 860);
    mpy_buff_to_bbus.addPoint(1240, 850);
    mpy_buff_to_bbus.addPoint(1230, 860);
    mpy_buff_to_bbus.draw(img);

    Branch pm_output; //branch going from program counter
    pm_output.addPoint(1730, 1210);
    pm_output.addPoint(1730, 1260);
    pm_output.addPoint(1720, 1250);
    pm_output.addPoint(1730, 1260);
    pm_output.addPoint(1740, 1250);
    pm_output.addPoint(1730, 1260);
    pm_output.addPoint(2130, 1260);
    pm_output.addPoint(2130, 960);
    pm_output.addPoint(1730, 960);
    pm_output.addPoint(1730, 1000);
    pm_output.addPoint(1720, 990);
    pm_output.addPoint(1730, 1000);
    pm_output.addPoint(1740, 990);
    pm_output.addPoint(1730, 1000);
    pm_output.addPoint(1730, 960);
    pm_output.addPoint(2130, 960);
    pm_output.addPoint(2130, 860);
    pm_output.addPoint(2090, 860);
    pm_output.addPoint(2130, 860);
    pm_output.addPoint(2130, 220);
    pm_output.addPoint(2090, 220);
    pm_output.draw(img);

    Branch pm_to_branch_control; //branch going from program memory to branch control
    pm_to_branch_control.addPoint(1730, 1210);
    pm_to_branch_control.addPoint(1730, 1260);
    pm_to_branch_control.addPoint(1230, 1260);
    pm_to_branch_control.addPoint(1230, 1080);
    pm_to_branch_control.addPoint(1220, 1090);
    pm_to_branch_control.addPoint(1230, 1080);
    pm_to_branch_control.addPoint(1240, 1090);
    pm_to_branch_control.addPoint(1230, 1080);
    pm_to_branch_control.addPoint(1230, 1260);
    pm_to_branch_control.addPoint(800, 1260);
    pm_to_branch_control.addPoint(810, 1270);
    pm_to_branch_control.addPoint(800, 1260);
    pm_to_branch_control.addPoint(810, 1250);
    pm_to_branch_control.draw(img);


    /********************* TEXT INPUT *****************************************************/
    cv::putText(img, "To Other Control Points", cv::Point(900, 1300), font, 0.8, black, 2);
    cv::putText(img, "emit field", cv::Point(2000, 1250), font, 0.8, black, 2);
    cv::putText(img, "A-bus", cv::Point(400, 840), font, 0.8, black, 2);
    cv::putText(img, "B-bus", cv::Point(1400, 840), font, 0.8, black, 2);
    cv::putText(img, "Data bus", cv::Point(1400, 210), font, 0.8, black, 2);
    cv::putText(img, "Address 1", cv::Point(940, 130), font, 0.8, black, 2);
    cv::putText(img, "Address 2", cv::Point(1400, 130), font, 0.8, black, 2);
    cv::putText(img, "load", cv::Point(1530, 1090), font, 0.8, black, 2);
    cv::putText(img, "Data 1", cv::Point(1030, 390), font, 0.8, black, 2);
    cv::putText(img, "Data 2", cv::Point(1350, 390), font, 0.8, black, 2);
    /***************************************************************************************/

    // ******** VARIABLES ********* //
    short pc = 0;
	short a_reg = 0;
	short ma1_reg = 0;
	short md1_reg = 0;
	short ma2_reg = 0;
	short md2_reg = 0;
	short loop_reg = 0;
	short A = 0;	//left side input of ALU	
	short B = 0;	//right side input of ALU
	short C = 0; 	//output of ALU
	short D = 0; 	//output of MPY

	bool ovf = false;
	bool zf = false;
	bool mf = false;
	bool pre_ovf = false;
	bool pre_z = false;
	bool pre_m = false;
	bool jump = false;

	short mem[nmax];
	std::copy(memory, memory + nmax, mem);
    short a_regi[8];
    short b_regi[8];
    std::copy(mem, mem + 8, a_regi);
    std::copy(mem + 8, mem + 16, b_regi);

    std::vector<short> a_vector(a_regi, a_regi + 8);
    std::vector<short> b_vector(b_regi, b_regi + 8);

	//memset(mem, 0, sizeof(mem));
	//short a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	//short b[8] = {1, 1, 1 ,1 ,1, 1, 1, 1};
	//memcpy(mem, a, sizeof(a));
	//memcpy(mem + 8, b, sizeof(b));



	/********** MICROPROGRAM EXECUTION LOOP **********/
    //********** CREATING WINDOW **********/
    drawPreviewMemory(img, a_vector, b_vector);
    showValue(img, "PC:", pc, 50, 1320);
    showValue(img, "Result:", mem[63], 50, 1360);
    showValue(img, "loop_reg:", loop_reg, 1100, 940, red);
    cv::namedWindow("img", cv::WINDOW_NORMAL);
    cv::imshow("img", img);
    cv::setMouseCallback("img", onMouse, &img);

    while(1){
        //std::cout << pc << std::endl;
    MIP now = mip[pc];

    if (now.stp != 0) {
        std::cout << "program end" << std::endl;
        break;
    }

    if (now.MA1l == 1) {
        if (now.MA1i == 1) {
            ma1_reg = C;
        } else if (now.MA1i == 0 && now.Deo == 0) {
            ma1_reg = now.emit;
        }
    }
    if (now.A1I == 1) {
        ma1_reg = ma1_reg + 1;
    }

    if (now.MA2l == 1 && now.Deo == 0) {
        ma2_reg = now.emit;
    }
    if (now.A2I == 1) {
        ma2_reg = ma2_reg + 1;
    }

    // memory read
    if (now.MD1l == 1) {
        if (now.MD1i == 0) {
            if (now.r_w == 1 && now.memo == 0) {
                if (ma1_reg < nmax && ma1_reg >= 0) {
                    md1_reg = mem[ma1_reg];
                } else {
                    std::cout << "memory read error : out of memory range" << std::endl;
                    exit(0);
                }
            }
        } else if (now.MD1i == 1) {
            if (now.Deo == 0) {
                md1_reg = now.emit;
            }
        } else if (now.MD1i == 2) {
            md1_reg = C;
        }
    }

    if (now.MD2l == 1) {
        if (now.MD2i == 0) {
            if (now.r_w == 1 && now.memo == 0) {
                if (ma2_reg < nmax && ma2_reg >= 0) {
                    md2_reg = mem[ma2_reg];
                } else {
                    std::cout << "memory read error : out of memory range" << std::endl;
                    exit(0);
                }
            }
        } else if (now.MD2i == 1) {
            if (now.Deo == 0) {
                md2_reg = now.emit;
            }
        }
    }

	//memory write
	if (now.r_w == 0) {
    	if (now.memo == 1) {
        	// std::cout << "memory write error : signal" << std::endl;
    	} else {
        if (now.MMD1o == 0) {
            if (ma1_reg < nmax && ma1_reg >= 0) {
                mem[ma1_reg] = md1_reg;
            } else {
                std::cout << "memory write error : out of memory range" << std::endl;
                exit(0);
            }
        	}
        if (now.MMD2o == 0) {
            if (ma2_reg < nmax && ma2_reg >= 0) {
                mem[ma2_reg] = md2_reg;
            } else {
                std::cout << "memory write error : out of memory range" << std::endl;
                exit(0);
            }
        	}
    	}
	}

	// A-bus control
	if ((now.Ao + now.MA1o + now.A1o + now.A0o + now.Byo) < 4) {
    	std::cout << "A-bus conflict error" << std::endl;
    	exit(0);
	}
	if (now.Ao == 0) {
    	A = a_reg;
	}
	if (now.MA1o == 0) {
    	A = ma1_reg;
	}
	if (now.A1o == 0) {
    	A = 1;
	}
	if (now.A0o == 0) {
    	A = 0;
	}
	if (now.Byo == 0) {
    	A = C;
	}

	// MPY
	D = mpy(md1_reg, md2_reg);

	// B-bus control
	if ((now.BMD1o + now.BMPYo + now.Beo) < 2) {
    	std::cout << "B-bus conflict error" << std::endl;
    	exit(0);
	}
	if (now.BMD1o == 0) {
    	B = md1_reg;
	}
	if (now.BMPYo == 0) {
    	B = D;
	}
	if (now.Beo == 0) {
		B = now.emit;
   	}

	// latch control
	if (now.Al == 1) {
    	a_reg = C;
	}
	
	/*****************************************************
	if (now.MA1l == 1) {
    	if (now.MA1i == 1) {
        	ma1_reg = C;
    	} else if (now.MA1i == 0 && now.Deo == 0) {
        	ma1_reg = now.emit;
    	}
	}

	if (now.A1I == 1) {
    	ma1_reg = static_cast<short>(ma1_reg + 1);
	}
	*****************************************************/

	if(now.MD1l == 1 && now.MD1i == 1 && now.Deo == 0) {
		md1_reg = now.emit;
	}
	/*****************************************************
	if (now.MA2l == 1 && now.Deo == 0) {
    	ma2_reg = now.emit;
	}
	if (now.A2I == 1) {
    	ma2_reg = static_cast<short>(ma2_reg + 1);
	}
	******************************************************/

	if(now.Lpl == 1 && now.Beo == 0) {
		loop_reg = now.emit;
	}

	if(now.Lpc == 1) {
		loop_reg = loop_reg - 1;
	}

	// check branch condition
	if ((now.ovf == 1 && ovf == true) || (now.zf == 1 && zf == true) || (now.mf == 1 && mf == true) || (now.ncld == 1)) {
    	jump = true;
	} else {
    	jump = false;
	}

	// ALU
	short pre_ovf = ovf;
	short pre_z = zf;
	short pre_m = mf;

	Result result = falu(now.alu, A, B);
	C = result.C;
	ovf = result.ovf;
	zf = result.z;
	mf = result.m;




	// check the range of pipeline register
	if (now.Lpc == 1 && loop_reg >= l_max) {
    std::cout << "out of range of loop register" << std::endl;
    std::exit(0);
	}

	printf("PC: %d\n", pc);
	printf("Al: %d, Ao: %d, A1o: %d, A0o: %d, MA1i: %d, MA1l: %d, A1I: %d, MA1o: %d, Byo: %d, MD1i: %d, MD1l: %d, BMD1o: %d, MD2i: %d, MD2l: %d, MA2l: %d, A2I: %d, BMPYo: %d, Lpl: %d, Lpc: %d, r_w: %d, memo: %d, MMD1o: %d, MMD2o: %d, alu: %d, Deo: %d, Beo: %d, ovf: %d, zf: %d, mf: %d, ncld: %d, stp: %d, emit: %d, mem[63]: %d\n", now.Al, now.Ao, now.A1o, now.A0o, now.MA1i, now.MA1l, now.A1I, now.MA1o, now.Byo, now.MD1i, now.MD1l, now.BMD1o, now.MD2i, now.MD2l, now.MA2l, now.A2I, now.BMPYo, now.Lpl, now.Lpc, now.r_w, now.memo, now.MMD1o, now.MMD2o, now.alu, now.Deo, now.Beo, now.ovf, now.zf, now.mf, now.ncld, now.stp, now.emit, mem[63]);
	
	printf("MEMORY CONTENT: \n");
	for (int i = 0; i < nmax; i++) {
		printf("%4d ", mem[i]);
		if(i % 8 == 7) {
			printf("\n");
		}
	}
    int key;
	// branch control
	if (jump) {
    	if (now.emit >= p_max) {
       		std::cout << "program crash" << std::endl;
        	std::exit(0);
    	} else {
        	pc = now.emit;
    	}
	} else {
    	if (now.Lpc == 1 && loop_reg != 0) {
        	pc = pc;
    	} else {
        	// program counter increment
                pc++;
    	}
    showValue(img, "PC:", pc, 50, 1320);
    showValue(img, "Result:", mem[63], 50, 1360);
    showValue(img, "loop_reg:", loop_reg, 1100, 940, red);
    if(pc == 1 || pc == 6) {
        ma1_selector.setColor(2);
        ma1_selector.selector(510,320);
        cv::imshow("img", img);
    } else {
        ma1_selector.setColor(0);
        ma1_selector.selector(510,320);
        cv::imshow("img", img);
    }

    if(pc==1||(pc==4 && loop_reg!=8) || pc==6){
        ma1_register.setColor(2);
        ma1_register.ma1_register(510, 400);
        cv::imshow("img", img);
    } else {
        ma1_register.setColor(0);
        ma1_register.ma1_register(510, 400);
        cv::imshow("img", img);
    }

    if(pc==1||pc==2||pc==6) {
        databus.setColor(2);
        databus.draw(img);
        cv::imshow("img", img);
    } else {
        databus.setColor(0);
        databus.draw(img);
        cv::imshow("img", img);
    }

    if(pc==1||pc==2||pc==3||(pc==4 && loop_reg==8) || pc==6) {
        pm_output.setColor(2);
        pm_output.draw(img);
        cv::imshow("img", img);
    } else {
        pm_output.setColor(0);
        pm_output.draw(img);
        cv::imshow("img", img);
    }

    if(pc==2 || pc==4&&loop_reg!=8) {
        ma2_register.setColor(2);
        ma2_register.ma2_register(1710, 400);
        cv::imshow("img", img);
    } else {
        ma2_register.setColor(0);
        ma2_register.ma2_register(1710, 400);
        cv::imshow("img", img);
    }

    if(pc==3||pc==4) {
        loop_register.setColor(1);
        loop_register.loop_register(1000, 900);
        b_bus.setColor(2);
        b_bus.draw(img);
        cv::imshow("img", img);
    } else {
        loop_register.setColor(0);
        loop_register.loop_register(1000, 900);
        b_bus.setColor(0);
        b_bus.draw(img);
        cv::imshow("img", img);
    }

    if(pc==4||pc==5) {
        alu.setColor(2);
        alu.alu(610, 900);
        cv::imshow("img", img);
    } else {
        alu.setColor(0);
        alu.alu(610, 900);
        cv::imshow("img", img);
    }

    if(pc==4&&loop_reg!=8) {
        ma1_to_data_memory.setColor(2);
        ma2_to_data_mem.setColor(2);
        data_mem_to_md1_sel.setColor(2);
        data_mem_to_md2_sel.setColor(2);
        multiplier.setColor(2);
        md2_selector.setColor(2);
        md2_register.setColor(2);
        md1_to_mpy.setColor(2);
        md2_to_mpy.setColor(2);
        //alu.setColor(2);
        mpy_to_buff.setColor(2);
        mpy_buff_to_bbus.setColor(2);

        ma1_to_data_memory.draw(img);
        ma2_to_data_mem.draw(img);
        data_mem_to_md1_sel.draw(img);
        data_mem_to_md2_sel.draw(img);
        multiplier.multiplier(1110, 660);
        md2_selector.selector(1350, 320);
        md2_register.md2_register(1350, 400);
        md1_to_mpy.draw(img);
        md2_to_mpy.draw(img);
        //alu.alu(610, 900);
        mpy_to_buff.draw(img);
        mpy_buff_to_bbus.draw(img);
        cv::imshow("img", img);
    } else {
        ma1_to_data_memory.setColor(0);
        ma2_to_data_mem.setColor(0);
        data_mem_to_md1_sel.setColor(0);
        data_mem_to_md2_sel.setColor(0);
        multiplier.setColor(0);
        md2_selector.setColor(0);
        md2_register.setColor(0);
        md1_to_mpy.setColor(0);
        md2_to_mpy.setColor(0);
        //alu.setColor(0);
        mpy_to_buff.setColor(0);
        mpy_buff_to_bbus.setColor(0);

        ma1_to_data_memory.draw(img);
        ma2_to_data_mem.draw(img);
        data_mem_to_md1_sel.draw(img);
        data_mem_to_md2_sel.draw(img);
        multiplier.multiplier(1110, 660);
        md2_selector.selector(1350, 320);
        md2_register.md2_register(1350, 400);
        md1_to_mpy.draw(img);
        md2_to_mpy.draw(img);
        //alu.alu(610, 900);
        mpy_to_buff.draw(img);
        mpy_buff_to_bbus.draw(img);
        cv::imshow("img", img);
    }



    if((pc==4&&loop_reg!=8) || pc == 7) {
        data_memory.setColor(2);
        data_memory.data_memory(1110, 100);
        cv::imshow("img", img);
    } else {
        data_memory.setColor(0);
        data_memory.data_memory(1110, 100);
        cv::imshow("img", img);
    }

    if(pc==1 ||(pc==4&&loop_reg!=8) || pc == 6) {
        ma1_register.setColor(2);
        ma1_register.ma1_register(510, 400);
        cv::imshow("img", img);
    } else {
        ma1_register.setColor(0);
        ma1_register.ma1_register(510, 400);
        cv::imshow("img", img);
    }

    if((pc==4&&loop_reg!=8) || pc == 5) {
        md1_selector.setColor(2);
        md1_selector.selector(870, 320);
        cv::imshow("img", img);
    } else {
        md1_selector.setColor(0);
        md1_selector.selector(870, 320);
        cv::imshow("img", img);
    }

    if((pc==4&&loop_reg!=8) || pc == 5 || pc==7) {
        md1_register.setColor(2);
        md1_register.md1_register(870, 400);
        cv::imshow("img", img);
    } else {
        md1_register.setColor(0);
        md1_register.md1_register(870, 400);
        cv::imshow("img", img);
    }

    if(pc==5) {
        alu_output.setColor(2);
        alu_output.draw(img);
        cv::imshow("img", img);
    } else {
        alu_output.setColor(0);
        alu_output.draw(img);
        cv::imshow("img", img);
    }

    if(pc==7) {
        md1_to_buffer.setColor(2);
        md1_buff_to_data_mem.setColor(2);
        md1_to_buffer.draw(img);
        md1_buff_to_data_mem.draw(img);
        cv::imshow("img", img);
    } else {
        md1_to_buffer.setColor(0);
        md1_buff_to_data_mem.setColor(0);
        md1_to_buffer.draw(img);
        md1_buff_to_data_mem.draw(img);
        cv::imshow("img", img);
    }

    cv::imshow("img", img);
    key = cv::waitKey(0);
    if (key == 'n' || key == 'N') {
        continue;
    } else if (key == 'q' || key == 'Q') {
        break;
    } else if (mip[pc].stp != 0) {
        break;
    }
	}
    }

    return result;
}