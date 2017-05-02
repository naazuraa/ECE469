#include "usertraps.h"
#include "misc.h"

#include "fdisk.h"

dfs_superblock sb;
dfs_inode inodes[FDISK_NUM_INODES];
uint32 fbv[DFS_FBV_MAX_NUM_WORDS];

int diskblocksize = 0; // These are global in order to speed things up
int disksize = 0;      // (i.e. fewer traps to OS to get the same number)

int FdiskWriteBlock(uint32 blocknum, dfs_block *b); //You can use your own function. This function 
//calls disk_write_block() to write physical blocks to disk

inline
void
SetFBV (int p, int b)
{
  uint32  wd = p / 32;
  uint32  bitnum = p % 32;

  fbv[wd] = (fbv[wd] & invert(1 << bitnum)) | (b << bitnum);
  dbprintf ('m', "Set fbv entry %d to 0x%x.\n", wd, fbv[wd]);
}

void main (int argc, char *argv[])
{
	// STUDENT: put your code here. Follow the guidelines below. They are just the main steps. 
	// You need to think of the finer details. You can use bzero() to zero out bytes in memory

  //Initializations and argc

  //Initiializations:
  int i,j;
  dfs_block new_block;
  //disksize = disk_size();
  //diskblocksize = disk_blocksize();


  //argc
  if (argc != 1) {
    Printf("Error incorrect arguement numbers");
  }
  // Need to invalidate filesystem before writing to it to make sure that the OS
  // doesn't wipe out what we do here with the old version in memory
  // You can use dfs_invalidate(); but it will be implemented in Problem 2. You can just do 
  // sb.valid = 0

  //dfs_invalidate();
  sb.valid = 0;
  sb.dfs_blocksize = DFS_BLOCK_SIZE;
  sb.dfs_numblocks = FDISK_NUM_BLOCKS;
  sb.dfs_start_block_inodes = FDISK_INODE_BLOCK_START;
  sb.num_inodes = FDISK_NUM_INODES;
  sb.dfs_start_block_fbv = FDISK_FBV_BLOCK_START;
  sb.dfs_start_block_data = FDISK_FBV_BLOCK_START + (DFS_FBV_MAX_NUM_WORDS * 4)/DFS_BLOCK_SIZE;



  disksize = DISK_SIZE;
  diskblocksize = DISK_BLOCKSIZE;
  //num_filesystem_blocks = 

  // Make sure the disk exists before doing anything else
  if (disk_create() == DISK_FAIL) {
    Printf("Unable to create disk");

  }

  // Write all inodes as not in use and empty (all zeros)
  // Next, setup free block vector (fbv) and write free block vector to the disk
  // Finally, setup superblock as valid filesystem and write superblock and boot record to disk: 
  // boot record is all zeros in the first physical block, and superblock structure goes into the second physical block

  //Initialize inodes
  for (i = 0; i < FDISK_NUM_INODES; i++) {
    inodes[i].inuse = 0;
    inodes[i].filesize = 0;
    inodes[i].indirect_block = 0;
    for (j = 0; j < 10; j++) {
      indodes[i].virt_blocks[j] = 0;
    }
  }

  //Write them in as 0
  for (i = FDISK_INODE_BLOCK_START; i < FDISK_INODE_NUM_BLOCKS; i++) {
    bzero(new_block.data, sb.dfs_blocksize);
    FdiskWriteBlock(i, &new_block);
  }

  //setup free block vector fbv
  for (i = 0; i < DFS_FBV_MAX_NUM_WORDS; i++) {
    fbv[i] = 0;
  }
  //free the data blocks
  for (i = sb.dfs_start_block_data; i < sb.dfs_numblocks; i++) {
    setFBV(i, 1);
  }

  //Write fbv to disk (block number 17 and 18)
  for (i = sb.dfs_start_block_fbv; i < sb.dfs_start_block_data; i++) {
    bzero(new_block.data, sb.dfs_blocksize);
    bcopy( &((char*)(fbv[i - sb.dfs_start_block_fbv * (sb.dfs_blocksize / 4)])), new_block.data, sb.dfs_blocksize);
    FdiskWriteBlock(i, &new_block);
  }

  //Set superblock as valid file system and write superblock and boot record to disk
  sb.valid = 1;

  //write superblock to block 0 in disk
  bzero(new_block.data, sb.dfs_blocksize);
  //must go into second physical block, so second half of dfs block
  bcopy((char *)(&sb), &(new_block[diskblocksize]), sizeof(sb));
  FdiskWriteBlock(0, &new_block);



  Printf("fdisk (%d): Formatted DFS disk for %d bytes.\n", getpid(), disksize);
}

int FdiskWriteBlock(uint32 blocknum, dfs_block *b) {
  // STUDENT: put your code here
  //calls disk_write_block() to write physical blocks to disk
  disk_block *db;
  int i, m;
  m = sb.dfs_blocksize / diskblocksize;
  for (i = 0; i < m; i++) {
    bcopy(&(b->data[i * diskblocksize]), &(db->data), diskblocksize)
    if (DiskWriteBlock(blocknum * m + i, db) == DISK_FAIL) {
      Printf("Unable to write physical block to disk");
      return DISK_FAIL;
    }
  }

  return DISK_SUCCESS;
}