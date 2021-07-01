from config import BOARD_LOCATION
import pyautogui

class Skill:
    """
    There are 6 characters and all of them have one skill. 
    By tapping the charcter card, the skill can be activated.
    Currently, it might be challenging to know if a skill is used.
    Now, it should only be used for basic things.
    """

    # this shouldn't be changed (probably never)
    numberOfSkills = 6
    usedSkills = 5
    skillOrder = [6, 5, 4, 3, 2, 1]
    # TODO: is this necessary
    skillStatus = [True, True, True, True, True, True]

    def useNextSkill(self):
        current = self.skillOrder[self.usedSkills]
        # calculate the location
        left, top, end_left, end_top = BOARD_LOCATION
        width = end_left - left
        height = end_top - top

        one = width / self.numberOfSkills
        half = one / 2
        
        # calculate the location of this skill
        skill_left = (one * current - half) + left
        skill_top = top - (height / 5)
        # print(skill_left, skill_top, height / 5)
        
        pyautogui.moveTo(skill_left, skill_top)
        pyautogui.leftClick()    
        pyautogui.mouseDown()
        pyautogui.mouseUp()

    def confirmSkill(self):
        """
        Make sure skill was used and update the current state
        """
        self.usedSkills += 1
        if self.usedSkills >= self.numberOfSkills:
            return
        self.skillStatus[self.usedSkills] = False

if __name__ == "__main__":
    skill = Skill()
    skill.useNextSkill()
    skill.confirmSkill()
