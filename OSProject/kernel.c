printString(char*);
readString(char*);
mod(int, int);
div(int, int);
readSector(char*, int);
handleInterrupt21(int, int, int, int);
readFile(char*);
executeProgram(char*, int);
terminate();
writeSector(char*, int);
deleteFile(char*);
writeFile(char*, char*, int);

main() {
	//printString("Hello Wolrd\0");

	//char line[80];
	//printString("Enter a line: \0");
	//readString(line);
	//printString(line);

	//char buffer[512];
	//readSector(buffer, 30);
	//printString(buffer);

	//makeInterrupt21();
	//interrupt(0x21, 0, 0, 0, 0);

	//char line[512];
	//char line[80];
	//makeInterrupt21();
	//interrupt(0x21, 2, line, 30);
	//interrupt(0x21, 1, line, 0, 0);
	//interrupt(0x21, 0, line, 0, 0);

	//char buffer[13312];
	//makeInterrupt21();
	//interrupt(0x21, 3, "messag\0", buffer, 0);
	//interrupt(0x21, 0, buffer, 0, 0);

	//char* prog = "shell\0";

	char prog[6];
	prog[0] = 's';
	prog[1] = 'h';
	prog[2] = 'e';
	prog[3] = 'l';
	prog[4] = 'l';
	prog[5] = '\0';
	makeInterrupt21();
	interrupt(0x21, 4, prog, 0x2000, 0);

	//interrupt(0x21, 4, "tstprg\0", 0x2000, 0);
	//interrupt(0x21, 4, "tstpr2\0", 0x2000, 0);

	//executeProgram("shell\0", 0x2000);
	//interrupt(0x21, 5, 0, 0, 0);


	//char buffer[13312];
	//makeInterrupt21();
	//interrupt(0x21, 7, "messag\0", 0, 0);
	//interrupt(0x21, 3, "messag\0", buffer, 0);
	//interrupt(0x21, 0, buffer, 0, 0);

	// int i=0;
	// char buffer1[13312];
	// char buffer2[13312];
	// buffer2[0]='h'; buffer2[1]='e'; buffer2[2]='l';  buffer2[3]='l';
	// buffer2[4]='o';
	// for(i=5; i<13312; i++) buffer2[i]=0x0;
	// makeInterrupt21();
	// interrupt(0x21,8, "testW\0", buffer2, 1);
	// interrupt(0x21,3, "testW\0", buffer1, 0);
	// interrupt(0x21,0, buffer1, 0, 0);

	// char buffer[13312];
	// makeInterrupt21();
	// interrupt(0x21, 3, "messag\0", buffer, 0);
	// interrupt(0x21, 8, "messgg\0", buffer, 1);
	// interrupt(0x21, 0, buffer, 0, 0);

	while(1);
}

printString(char* chars) {
	char* ptr;

	for (ptr = chars; *ptr != '\0'; ptr++) {
		char c = *ptr;
		interrupt(0x10, 0xE*256+c, 0, 0, 0);
	}
}

readString(char* chars) {
	int i = 0;

	while(1) {
		char c = interrupt(0x16, 0, 0, 0, 0);
		
		// Character is ENTER:
		if (c == 0xd) {
			// chars[i] = 0xa;
			// i++;
			// chars[i] = 0x0;
			// interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
			// interrupt(0x10, 0xE*256+c, 0, 0, 0);
			chars[i++] = '\n';
			interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
			chars[i++] = '\r';
			interrupt(0x10, 0xE*256+c, 0, 0, 0);
			chars[i] = '\0';

			return;
		}

		// Character is BACKSPACE:
		else if (c == 0x8) {
			if (i > 0) {
				interrupt(0x10, 0xE*256+c, 0, 0, 0);
				interrupt(0x10, 0xE*256+' ', 0, 0, 0);
				interrupt(0x10, 0xE*256+c, 0, 0, 0);
				i--;
			}
		}

		// Character is any other KEY:
		else {
			chars[i] = c;
			interrupt(0x10, 0xE*256+c, 0, 0, 0);
			i++;
		}
	}
}

mod(int a, int b) {
	int temp = div(a, b);
	return a - temp * b;
}

div (int a, int b) {
	int result = 0;

	while ((a = a - b) >= 0) {
		result++;
	}
	return result;
}

readSector(char* buffer, int sector) {
	int relSector = mod(sector, 18) + 1;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36);

	interrupt(0x13, 2*256+1, buffer, track*256+relSector, 
		head*256+0);
}

handleInterrupt21(int ax, int bx, int cx, int dx) {
	//printString("Hello World!\0");
	//char *dgb = "AX = x\n";
	//dgb[5] = '0' + ax;
	//printString(dgb);
	switch(ax) {
		case 0: printString(bx); break;
		case 1: readString(bx); break;
		case 2: readSector(bx, cx); break;
		case 3: readFile(bx, cx); break;
		case 4: executeProgram(bx, cx); break;
		case 5: terminate(); break;
		case 6: writeSector(bx, cx); break;
		case 7: deleteFile(bx); break;
		case 8: writeFile(bx, cx, dx); break;
		default: printString("Error!");
	}
}

readFile(char* fileName, char* bufferAdr) {
	char dir[512];
	int i;
	int j;
	readSector(dir, 2);

	for(i=0; i<512; i=i+0x20) {
		for (j=0; j<6; j++) {
			if(dir[i+j] != fileName[j])
				goto continue_outer_loop;
		}
		j = i+6;
		while (dir[j] != 0x00) {
			//make a copy, or use Ramy's formula
			readSector(bufferAdr, dir[j]);
			bufferAdr += 512;
			j++;
		}
		continue_outer_loop:;
	}
	//printString("readFile\r\n");
}

executeProgram(char* name, int segment) {
	int i;
	char buffer[13312];
	//printString("executeProgram\r\n");
	readFile(name, buffer);
	if (buffer[0] == 0x00) {
		//Check with Ramy
		interrupt(0x21, 0, "File not found!\0", 0, 0);
		interrupt(0x21, 5, 0, 0, 0);
	}
	//printString("executeProgram READ\r\n");
	for(i=0; i<13312; i++) {
		putInMemory(segment, i, buffer[i]);
	}
	//printString("executeProgram MEM\r\n");
	launchProgram(segment);
}

terminate() {
	//char* prog = "shell\0";
	//printString("terminate\r\n");
	//while(1) {
		//interrupt(0x21, 4, "shell\0", 0x2000, 0);
		//executeProgram("shell\0", 0x2000);
	//}
	//makeInterrupt21();
	char prog[6];
	prog[0] = 's';
	prog[1] = 'h';
	prog[2] = 'e';
	prog[3] = 'l';
	prog[4] = 'l';
	prog[5] = '\0';
	interrupt(0x21, 4, prog, 0x2000, 0);
	//executeProgram("shell\0", 0x2000);
}

writeSector(char* buffer, int sector) {
	int relSector = mod(sector, 18) + 1;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36);

	interrupt(0x13, 3*256+1, buffer, track*256+relSector, 
		head*256+0);
}

deleteFile(char* name) {
	char map[512];
	char dir[512];
	int i;
	int j;
	int sector;

	readSector(map, 1);
	readSector(dir, 2);

	for(i=0; i<512; i=i+0x20) {
		for (j=0; j<6; j++) {
			if(dir[i+j] != name[j])
				goto continue_outer_loop;
		}
		dir[i] = 0x00;
		j = i+6;
		while (dir[j] != 0x00) {
			sector = dir[j++];
			map[sector+1] = 0x00;
		}
		continue_outer_loop:;
	}
	writeSector(map, 1);
	writeSector(dir, 2);
}

writeFile(char* name, char* buffer, int secNum) {
	char map[512];
	char dir[512];
	int i;
	int j;
	int k;
	int z;
	int remBytes;
	char data[512];

	readSector(map, 1);
	readSector(dir, 2);

	for(i=0; i<512; i=i+0x20) {
		if (dir[i] != 0x00) {
			goto continue_outer_loop;
		}
		j = 0;
		k = i;
		while (name[j] != '\0') {
			dir[i++] = name[j++];
		}
		z = i-k;
		if (z > 0) {
			while (z < 6) {
				dir[i++] = 0x00;
				z++;
			}
		}
		k = 0;
		z = 0;
		while (k < secNum) {
			while(z<512) {
				if (map[z] == 0x00) {
					map[z] = 0xff;
					break;
				}
				z++;
			}
			dir[i++] = z;
			for (j=0; j<512; j++) {
				data[j] = buffer[j+k*512];
			}
			writeSector(data, z);
			k++;
		}
		remBytes = 26-secNum;
		for(j=0; j<remBytes; j++) {
			dir[i++] = 0x00;
		}
		break;

		continue_outer_loop:;
	}
	writeSector(map, 1);
	writeSector(dir, 2);
}