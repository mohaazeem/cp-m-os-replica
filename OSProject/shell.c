getSectors(char*);

main() {
	char fileName1[7];
	char fileName2[7];
	char buffer1[13312];
	char buffer2[13312];
	char dir[512];
	char line[80];
	int i;

	while(1) {
		//int i;
		for (i=0; i<13312; i++) {
			buffer1[i] = 0x00;
			buffer2[i] = 0x00;
		}
		for (i=0; i<6; i++) {
			fileName1[i] = 0x00;
			fileName2[i] = 0x00;
		}
		for (i=0; i<512; i++) {
			dir[i] = 0x00;
		}
		for (i=0; i<80; i++) {
			line[i] = 0x00;
		}

		interrupt(0x21, 0, "SHELL>\0", 0, 0);
		interrupt(0x21, 1, line, 0, 0);

		if (line[0] == 'v' && line[1] == 'i' && line[2] == 'e' && line[3] == 'w') {
			//ask about file names < 6 characters
			//and file names that do not exist **
		//	int i;
			for (i=0; i<6; i++) {
				fileName1[i] = line[5+i];
			}
			fileName1[6] = '\0';
			interrupt(0x21, 3, fileName1, buffer1, 0);
			interrupt(0x21, 0, buffer1, 0, 0);
			if (buffer1[0] == 0x00) {
				interrupt(0x21, 0, "File not found!\n\r", 0, 0);
			}
		}
		else if (line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && line[3] == 'c'
			 && line[4] == 'u' && line[5] == 't' && line[6] == 'e') {
			//int i;
			for (i=0; i<6; i++) {
				fileName1[i] = line[8+i];
			}
			fileName1[6] = '\0';
			interrupt(0x21, 4, fileName1, 0x2000, 0);
			//interrupt(0x21, 5, 0, 0, 0);
		}
		else if(line[0] == 'd' && line[1] == 'e' && line[2] == 'l' && line[3] == 'e'
			 && line[4] == 't' && line[5] == 'e') {
		//	int i;
			for (i=0; i<6; i++) {
				fileName1[i] = line[7+i];
			}
			fileName1[6] = '\0';
			interrupt(0x21, 7, fileName1, 0, 0, 0);
			//interrupt(0x21, 5, 0, 0, 0);
		}
		else if(line[0] == 'c' && line[1] == 'o' && line[2] == 'p' && line[3] ==
		 'y') {
		 	int c;
			for (i=0; i<6; i++) {
				fileName1[i] = line[5+i];
			}
			fileName1[6] = '\0';
			for (i=0; i<6; i++) {
				fileName2[i] = line[12+i];
			}
			fileName2[6] = '\0';
			//interrupt(0x21, 0, "hello\0", 0, 0);
			//interrupt(0x21, 0, fileName1, 0, 0);
			// for(i=0; i<512; i=i+0x20) {
			// 		for (j=0; j<6; j++) {
			// 			if(dir[i+j] != fileName1[j])
			// 				//interrupt(0x21, 0, "hello\0", 0, 0);
			// 				goto continue_outer_loop;
			// 		}
			// 		//interrupt(0x21, 0, "hello\0", 0, 0);
			// 		j = i+6;
			// 		//interrupt(0x21, 0, "hello\0", 0, 0);
			// 		while (dir[j] != 0x00 && j < i + 32) {
			// 			c++;
			// 			j++;
			// 		}
			// 		//interrupt(0x21, 0, "hello\0", 0, 0);
			// 		break;

			// 		continue_outer_loop:;
			// 	}
			
			//c = getSectors(fileName1);
			interrupt(0x21, 3, fileName1, buffer1, 0);
			interrupt(0x21, 8, fileName2, buffer1, 1);
		}
		else {
			interrupt(0x21, 0, "Command not found!\n\r", 0, 0);
		}
	}
}

getSectors(char* fileName) {
	int i;
	int j;
	int c;
	char dir[512];
	for (i=0; i<512; i++) {
		dir[i] = 0x00;
	}
	interrupt(0x21, 2, dir, 2, 0);
	for(i=0; i<512; i=i+0x20) {
		for (j=0; j<6; j++) {
			if(dir[i+j] != fileName[j])
				//interrupt(0x21, 0, "hello\0", 0, 0);
				goto continue_outer_loop;
		}
		//interrupt(0x21, 0, "hello\0", 0, 0);
		j = i+6;
		//interrupt(0x21, 0, "hello\0", 0, 0);
		while (dir[j] != 0x00 && j < i + 32) {
			c++;
			j++;
		}
		//interrupt(0x21, 0, "hello\0", 0, 0);
		break;

		continue_outer_loop:;
	}
	return c;
}