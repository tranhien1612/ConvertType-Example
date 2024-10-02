#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <dirent.h>

typedef struct coordinates{ //N, O, A, P
    double Nx;
    double Ny;
    double Nz;
    double Ox;
    double Oy;
    double Oz;
    double Ax;
    double Ay;
    double Az;
    double Px;
    double Py;
    double Pz;
}coordinates_t;

typedef struct msg_t{ //N, O, A, P
    uint8_t type;
    double V;
    coordinates_t coordinate;
}msg_t;

#define MOVL "MOVL"
#define MOVJ "MOVJ"
#define M_PI 3.14159265358979323846
#define Xtcp 1096
#define Ytcp 0
#define Ztcp 1109

FILE *fileRead;
FILE *fileWrite;

int count = 1;
void write2file(msg_t msg){
    char text[300];
    sprintf(text, "%s=%.01f PL=1 ACC=0.0 DEC=0 TOOL=4 BASE=0 USE=1 COUNT=0 " \
                "Nx=%.10f Ny=%.10f Nz=%.10f " \
                "Ox=%.10f Oy=%.10f Oz=%.10f " \
                "Ax=%.10f Ay=%.10f Az=%.10f " \
                "Px=%.10f Py=%.10f Pz=%.10f N%d\n" , 
        (msg.type == 0) ? "MOVL VL" : "MOVJ VJ", msg.V, 
        msg.coordinate.Nx, msg.coordinate.Ny, msg.coordinate.Nz, msg.coordinate.Ox, msg.coordinate.Oy, msg.coordinate.Oz, 
        msg.coordinate.Ax, msg.coordinate.Ay, msg.coordinate.Az, msg.coordinate.Px, msg.coordinate.Py, msg.coordinate.Pz, count++);

    if (fprintf(fileWrite, "%s", text) < 0) {
        perror("Error writing to file");
        fclose(fileWrite);
        return;
    }
}

coordinates_t calculate(double x, double y, double z, double r, double p, double w){
    coordinates_t robot;
/*
    // double a = r*M_PI/180;
    // double b = (p+90)*M_PI/180; // b = p*M_PI/180;
    // double c = w*M_PI/180;
    // double ca = cos(a);
    // double sa = sin(a);
    // double cb = cos(b);
    // double sb = sin(b);
    // double cc = cos(c);
    // double sc = sin(c);

    // robot.Nx = cb * cc;
    // robot.Ny = sa * sb * cc + ca * sc;
    // robot.Nz = -(-ca * sb * cc + sa * sc);
    // robot.Ox = -cb * sc;
    // robot.Oy = -sa * sb * sc + ca * cc;
    // robot.Oz = -(ca * sb * sc + sa * cc);
    // robot.Ax = -sb;
    // robot.Ay = -sa * cb;
    // robot.Az = ca * cb;
    // robot.Px = x-( cb * cc*Xtcp + (-cb * sc)*Ytcp + (-sb*Ztcp));
    // robot.Py = y-((sa * sb * cc + ca * sc)*Xtcp + (-sa * sb * sc + ca * cc)*Ytcp + -sa *cb*Ztcp);
    // robot.Pz = z-(-(-ca * sb * cc + sa * sc)*Xtcp + -(ca * sb * sc + sa * cc)*Ytcp + (ca * cb)*Ztcp);
*/
    
    double a = r*M_PI/180;
    double b = (p+90)*M_PI/180;
    double c = w*M_PI/180;
    double ca = cos(a);
    double sa = sin(a);
    double cb = cos(b);
    double sb = sin(b);
    double cc = cos(c);
    double sc = sin(c);

    robot.Nx = cb * cc;
    robot.Ny = sa * sb * cc + ca * sc;
    robot.Nz = (-ca * sb * cc + sa * sc);

    robot.Ox = -cb * sc;
    robot.Oy = -sa * sb * sc + ca * cc;
    robot.Oz = (ca * sb * sc + sa * cc);

    robot.Ax = sb;
    robot.Ay = -sa * cb;
    robot.Az = ca * cb;

    robot.Px = x - ( cb * cc*Xtcp + (-cb * sc)*Ytcp + (sb*Ztcp));
    robot.Py = y - ((sa * sb * cc + ca * sc)*Xtcp + (-sa * sb * sc + ca * cc)*Ytcp + (-sa *cb)*Ztcp);
    robot.Pz = z - ((-ca * sb * cc + sa * sc)*Xtcp + (ca * sb * sc + sa * cc)*Ytcp + (ca * cb)*Ztcp);
    return robot;
}

void get_param(const char *line){
    // printf("%s", line);
    char *input_copy = strdup(line);
    char *token = strtok(input_copy, " ");

    uint8_t type;
    double V;
    double A[6];
    if(strcmp(token, "MOVL") == 0){
        type = 0;
    }else if(strcmp(token, "MOVJ") == 0){
        type = 1;
    }

    char* ver = strtok(NULL, " ");
    for(int i = 0; i < 6; i++){
        token = strtok(NULL, " ");
        if(token == NULL) return;
        A[i] = atof(token);
    }
    char *token1 = strtok(ver, "=");
    token1 = strtok(NULL, "=");
    V = atof(token1);

    free(input_copy);

    msg_t message;
    message.coordinate = calculate(A[0], A[1], A[2], A[3], A[4], A[5]);
    message.type = type;
    message.V = V;
    write2file(message);
}

void readFile(){
    char* ret;
    char line[256];  // Buffer to store each line
    while (fgets(line, sizeof(line), fileRead) != NULL) {
        ret = strstr(line, "MOV");
        if(ret != NULL) 
            get_param(line);
        else{
            if (fprintf(fileWrite, "%s", line) < 0) {
                perror("Error writing to file");
                fclose(fileWrite);
                return;
            }
        }
    }
}

void parse_commandline(int argc, char **argv, char *&fileReadName, char *&directoryPath, int &type){
    // char *fileReadName;
    // char *fileWriteName;
    const char *commandline_usage = "usage: main [-i <input_filename>]\n" \
                                    "       main [-f <folder_path>]\n";
    // if(argc < 5){
    //     printf("%s\n", commandline_usage);
    //     exit(0);
    // }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
			if (argc > i + 1) {
				i++;
                fileReadName = argv[i];
                type = 1;
			} else {
				printf("%s\n",commandline_usage);
				throw EXIT_FAILURE;
			}
		}

        // if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
		// 	if (argc > i + 1) {
		// 		i++;
		// 		fileWriteName = argv[i];
		// 	} else {
		// 		printf("%s\n",commandline_usage);
		// 		throw EXIT_FAILURE;
		// 	}
		// }

        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--folder") == 0) {
			if (argc > i + 1) {
				i++;
                directoryPath = argv[i];
                type = 2;
			} else {
				printf("%s\n",commandline_usage);
				throw EXIT_FAILURE;
			}
		}
    }
}

void oneFile_process(char *fileName){
    fileRead = fopen(fileName, "r"); //fileRead
    strtok(fileName, ".");
    fileWrite = fopen(fileName, "w"); // fileWrite
    if (fileRead == NULL || fileWrite == NULL) {
        perror("Error opening file");
        return;
    }
    readFile();
    fclose(fileRead);
    fclose(fileWrite);
}

void open_folder(char *&directoryPath ){
    DIR *dp = opendir(directoryPath);

    if (dp == NULL) {
        perror("opendir error");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            if (strstr(entry->d_name, ".txt") != NULL) {
                oneFile_process((char*)entry->d_name);
            }
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[]) {
    char *directoryPath = (char*)"/home/maj/testFile";
    char *fileName = (char*)"data.txt";
    int type = 0;
    parse_commandline(argc, argv, fileName, directoryPath, type);
    if(type == 1){
        oneFile_process(fileName);
    }else if(type == 2)
        open_folder(directoryPath);

    // char *fileReadName = (char*)"test.txt";
    // char *fileWriteName = (char*)"path";
    // parse_commandline(argc, argv, fileReadName, fileWriteName);
    // // const char *commandline_usage = "usage: main [-i <input_filename> -o <output_filename>]";

    // fileRead = fopen(fileReadName, "r"); //fileRead
    // fileWrite = fopen(fileWriteName, "w"); // fileWrite

    // if (fileRead == NULL || fileWrite == NULL) {
    //     perror("Error opening file");
    //     return -1;
    // }
    // readFile();

    // fclose(fileRead);
    // fclose(fileWrite);

    return EXIT_SUCCESS;
}


