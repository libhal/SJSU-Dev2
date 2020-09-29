# from pandas import *
# from numpy import *
left = [
        [21, 7],   # segment 0, horizontal
        [44, 9],   # segment 1, vertical
        [44, 34],  # segment 2, vertical
        [21, 57],  # segment 3, horizontal
        [19, 34],  # segment 4, vertical
        [19, 9],   # segment 5, vertical
        [21, 32],  # segment 6, horizontal
        [55, 54]   # period following after
    ]
right = [
        [86, 7],    # segment 0, horizontal
        [109, 9],   # segment 1, vertical
        [109, 34],  # segment 2, vertical
        [86, 57],   # segment 3, horizontal
        [84, 34],   # segment 4, vertical
        [84, 9],    # segment 5, vertical
        [86, 32],   # segment 6, horizontal
        [120, 54]   # period following after
    ]

kBitmap = [
        "00111111",
        "00000110",
        "01011011",
        "01001111",
        "01100110",
        "01101101",
        "01111101",
        "00000111",
        "01111111",
        "01100111"
      ]

pixel_arr = [[0]*128 for i in range(64)]

def DrawVertical(x, y):
    for i in range(22):
        if (i == 0 or i == 21):
            pixel_arr[x+i][y] = 1

        elif (i == 1 or i == 20):
            pixel_arr[x+i][y-1] = 1
            pixel_arr[x+i][y] = 1
            pixel_arr[x+i][y+1] = 1

        elif (i >= 2 and i <= 19):
            pixel_arr[x+i][y-2] = 1
            pixel_arr[x+i][y-1] = 1
            pixel_arr[x+i][y] = 1
            pixel_arr[x+i][y+1] = 1
            pixel_arr[x+i][y+2] = 1

def DrawHorizontal(x, y):
    for i in range(22):
        if (i == 0 or i == 21):
            pixel_arr[x][y+i] = 1

        elif (i == 1 or i == 20):
            pixel_arr[x-1][y+i] = 1
            pixel_arr[x][y+i] = 1
            pixel_arr[x+1][y+i] = 1
            
        elif (i >= 2 and i <= 19):
            pixel_arr[x-2][y+i] = 1
            pixel_arr[x-1][y+i] = 1
            pixel_arr[x][y+i] = 1
            pixel_arr[x+1][y+i] = 1
            pixel_arr[x+2][y+i] = 1

def DrawCharacter(character):
    if(character < 10):
        for index, bit in enumerate(kBitmap[character][::-1]):
            if(int(bit) == 1):
                if(index == 0 or index == 3 or index == 6):
                    DrawHorizontal(right[index][1], right[index][0])
                else:
                    DrawVertical(right[index][1], right[index][0])
    if (character >= 10):
        first = 0
        second = 0
        if(character == 10):
            first = 1
            second = 0
        else:
            second = int(character%10)
            first = int(character/10)
        for index, bit in enumerate(kBitmap[first][::-1]):
            if(int(bit) == 1):
                if(index == 0 or index == 3 or index == 6):
                    DrawHorizontal(left[index][1], left[index][0])
                else:
                    DrawVertical(left[index][1], left[index][0])

        for index, bit in enumerate(kBitmap[second][::-1]):
            if(int(bit) == 1):
                if(index == 0 or index == 3 or index == 6):
                    DrawHorizontal(right[index][1], right[index][0])
                else:
                    DrawVertical(right[index][1], right[index][0])

def FileWriteout(file_obj):
    
    file_obj.write('{')
    for row in pixel_arr:
        r = str(row)
        r = r.replace('[', '{')
        r = r.replace(']', '},')
        file_obj.write(r)
        file_obj.write('\n')

    file_obj.write('}')

    file_obj.close()

file_object = open("d:\pixels.txt", "w+")
DrawCharacter(10)
FileWriteout(file_object)
file_object = open("d:\pixels1.txt", "w+")
DrawCharacter(88)
FileWriteout(file_object)
